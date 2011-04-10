#define KERNEL


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
    printk("Starting main");
    enable_interrupt();
    enable_cons_irq();
   
    //enable_mmu();
    ksetup_memory();

    char* swi_stack = kget_pages(10);
    set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);
   
    kinit_sched();
    
    memset(input, 0, 500);
    inputpos = 0;

    knew_proc(main, exit);
    knew_proc(main2, exit);
    proc *p = ksched();
    restore_pcb(&p->pcb);
}



int _ksyscall (int code, int r1, int r2, int r3) 
{
    switch (code) {
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
            return (int)kget_pages(r1);
        case FREE_PAGES:
            kfree_page((void*)r1);
            return 0;
        case EXIT:
            printk("Proc %d exiting", cp()->pid);
            kfree_proc(cp());
            return -1;
        default:
            printk("Bad syscall code %d", code);
            return -1;
    }
}

PCB* ksyscall (void* stacked_pcb) {
    kcopy_pcb(stacked_pcb);
    PCB* pcb = &cp()->pcb;
    int ret = _ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
    // ret < 0 means that the process doesn't exist anymore
    // or something else went wildly wrong
    if (ret >= 0) {
        pcb->r0 = ret;
    }
    proc *p = ksched();
    return &p->pcb;
}
