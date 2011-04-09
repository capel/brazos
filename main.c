#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;


void __attribute__ ((interrupt ("IRQ"))) irq_handler (void) {
    newchar = true;
}

void __attribute__ ((interrupt ("IRQ"))) dab_handler (void) {
    halt();
}

char update_input()
{
    char lastc = 0;
    char c;
    for (;;) {
        c = kgetch();
        if (!c)
            return lastc;

        if (c == '\r')
            c = '\n';

        if (inputpos < INPUTBUFSIZE -2) { 
            input[inputpos++] = c;
            input[inputpos] = '\0';
        }
     //   kputch(c);
        lastc = c;
    }
}


void kmain(void)
{
    printk("Starting main\n");
    enable_interrupt();
    enable_cons_irq();
   
    get_cpuid();

    //enable_mmu();
    ksetup_memory();

    char* swi_stack = kget_pages(10);
    char* user_stack = kget_pages(10);

    

    memset(input, 0, 500);
    inputpos = 0;

    set_swi_stack(swi_stack+PAGE_SIZE, 0,0,0);

    switch_to_user(user_stack+PAGE_SIZE, exit, main,0);
}

void __attribute__ ((interrupt ("SWI"))) swi_handler (int r0, int r1, int r2, int r3) 
{
    switch (r0) {
        case READ_STDIN:
            for (;;) {
                char c = update_input();
                if (c) break;
            }
            strlcpy((char*)r1, input, r2);
            inputpos = 0;
            r0 = 0;
            break;
        case WRITE_STDOUT:
            kputs((char*)r1);
            r0 = 0;
            break;
        case HALT:
            halt();
            r0 =0;
            break;
        case GET_PAGES:
            r0 = (int)kget_pages(r1);
            break;
        case FREE_PAGES:
            kfree_page((void*)r1);
            r0 = 0;
            break;
    }
    asm volatile ("mov r5, %[r0]"::[r0]"r"(r0));
}



