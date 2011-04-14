#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"
#include "kexec.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

PCB* kirq (PCB* stacked_pcb) {
    newchar = true;
    return stacked_pcb;
}

int update_input()
{
    int c = kgetc();
    switch(c) {
        case '\r':
            c = '\n';
            kputc(c);
            return c;
            break;
        case '\b':
            kputc('\b');
            kputc(' ');
            kputc('\b');
            return '\b';
        case 27:
            c = kgetc();
            if (c == 91) {
                switch (kgetc()) {
                    case 'D': return ARROW_LEFT;
                    case 'C': return ARROW_RIGHT;
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    default : return BAD_CODE;
                }
            } else {
                return BAD_CODE;
            }
        default:
            break;
    }

    kputc(c);

    /*if (!isalpha(c) && !isdigit(c)) {
        printk("<%d>", c);
    }*/
    
    return c;

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

    knew_proc(sh_main, exit);
    //knew_proc(main2, exit);
    proc *p = ksched();
    restore_pcb(&p->pcb);
}



int _ksyscall (int code, int r1, int r2, int r3) 
{
    void* prog_addr;
    switch (code) {
        case PUTC:
            kputc(r1);
            return 0;
        case GETC:
            for (;;) {
                int c = update_input();
                if (c) return c;
            }
        case WRITE_STDOUT:
            kputs((char*)r1);
            return 0;
        case HALT:
            khalt();
            return 0;
        case GET_PAGES:
            return (int)kget_pages_for_user(cp(), r1);
        case EXIT:
            printk("Proc %d exiting", cp()->pid);
            kfree_proc(cp());
            return -1;
        case YIELD:
            return 0;
        case EXEC:
            prog_addr = program_lookup((char*)r1);
            if (!prog_addr) {
                return 0;
            } else {
                kexec_proc(cp(), prog_addr, exit);
                return 0;
            }
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
