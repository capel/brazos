#define KERNEL

#include "include/common.h"
#include "include/sched.h"
#include "include/kexec.h"
#include "include/kfs.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

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
        /*
            kputc('\b');
            kputc(' ');
            kputc('\b');
        */
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

void setup(void) {
    ksetup_fs();
    ksetup_sched();
}

void kmain(void)
{
    printk("Starting main");
    enable_interrupt();
    enable_cons_irq();
   
    //enable_mmu();

    ksetup_memory();

    printk("setup mem done");
    AK = kcreate_actor(ACTOR_KERNEL);

    void* swi_stack;
    size_t ignore;
    pages* pp = kget_pages(10);
    MAP(pp, AK, P_W, &ignore, &swi_stack);
    set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);
   
    setup();

    memset(input, 0, 500);
    inputpos = 0;

    proc *p = ksched();
    restore_pcb(&PROC_PCB(p));
}


