#define KERNEL

#include "include/common.h"
#include "include/sched.h"
#include "include/kexec.h"
#include "include/kfs.h"
#include "include/mach.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

extern void sh_main();

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
    kcreate_root();
    ent * sch = kcreate_sched();
    assert(SUCCESS == LINK_R(root(), sch, "proc"));
    ent* mm = kcreate_mm();
    assert(SUCCESS == LINK_R(root(), mm, "mm"));
}

void kmain(void)
{
    printk("Starting main");
    enable_interrupt();
    enable_cons_irq();
   
    //enable_mmu();

    ksetup_memory();

    printk("setup mem done");

    printk("About to setup.");
    setup();
    printk("Done with setup.");


    void* swi_stack;
    size_t ignore;
    pages* pp = FS("/mm/new!/10");
    printk("pp %p", pp);
    MAP(pp, P_W, &ignore, &swi_stack);
    printk("Mapped swi to %p", swi_stack);
    set_swi_stack(swi_stack+10*PAGE_SIZE,0,0,0);
  

    memset(input, 0, 500);
    inputpos = 0;

    FS("/proc/new!");
    printk("Made proc");

    proc *p = FS("/proc/sched!");
    printk("sched proc %p", p);
    PROC_PCB(p).pc = sh_main;
    printk("Proc %p", p);
    restore_pcb(&PROC_PCB(p));
}


