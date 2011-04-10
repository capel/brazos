#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"


#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

PCB* kirq (PCB* stacked_pcb) {
    newchar = true;
    return stacked_pcb;
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
    set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);
   
    kinit_sched();
    
    memset(input, 0, 500);
    inputpos = 0;

    knew_proc(main, exit);
    restore_pcb(&ksched()->pcb);
}



int _ksyscall (int r0, int r1, int r2, int r3) 
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
        case EXIT:
            printk("Proc %d exiting", cp()->pid);
            kfree_proc(cp());
            return 0;
        default:
            return -1;
    }
}

PCB* ksyscall (void* stacked_pcb) {
    printk("stacked pcb: %p, cp %x, cp->pcb %x",
        stacked_pcb, cp(), &cp()->pcb);
    kcopy_pcb(stacked_pcb);
    PCB* pcb = &cp()->pcb;
    printk("ks cur spsr %x", pcb->spsr);
    int temp = _ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
    printk("ks2 cur spsr %x", pcb->spsr);
    pcb->r0 = temp;
    proc *p = ksched();
    printk("proc %p", p);
    return &p->pcb;
}
