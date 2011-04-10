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

PCB *pcb;

#define PCB_TO_ASM(pcb) (struct PCB*)(((char*)(pcb))+sizeof(PCB))
#define PCB_FROM_ASM(pcb) (struct PCB*)(((char*)(pcb))-sizeof(PCB))

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
    char* pcb_space = kget_page();
    pcb =(PCB*) pcb_space;
    
    memset(input, 0, 500);
    inputpos = 0;

    set_swi_stack(swi_stack+PAGE_SIZE, 0,0,0);

    switch_to_user(user_stack+PAGE_SIZE, exit, main,0);
}

void* syscall_handler (void* pcb_top) {
    memcpy((char*)pcb, (char*)pcb_top, sizeof(PCB));
    int temp = ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
    pcb->r0 = temp;
    return pcb;
}


int ksyscall (int r0, int r1, int r2, int r3) 
{
    switch (r0) {
        case READ_STDIN:
            for (;;) {
                char c = update_input();
                if (c) break;
            }
            strlcpy((char*)r1, input, r2);
            inputpos = 0;
            return 0;
        case WRITE_STDOUT:
            kputs((char*)r1);
            return 0;
        case HALT:
            khalt();
            return 0;
        case GET_PAGES:
            r0 = (int)kget_pages(r1);
            return r0;
        case FREE_PAGES:
            kfree_page((void*)r1);
            return 0;
        default:
            return -1;
    }
}

