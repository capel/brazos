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


void kmain(void)
{
    printk("Starting main");
    enable_interrupt();
    enable_cons_irq();
   
    //enable_mmu();
    ksetup_memory();
    ksetup_fs();

    printk("setup mem done");

    char* swi_stack = kget_pages(10);
    set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);
   
    kinit_sched();
    
    printk("init sched done");

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
    proc* new_proc;
    int ret;
    kfile *f;

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
            return -255;
        case YIELD:
            return 0;
        case EXEC:
            prog_addr = program_lookup((char*)r1);
            printk("Proc %d execing %s (%p)", cp()->pid, r1, prog_addr);
            if (!prog_addr) {
                return 0;
            } else {
                kexec_proc(cp(), prog_addr, exit);
                return 0;
            }
        case FORKEXEC:
            prog_addr = program_lookup((char*)r1);
            printk("Proc %d execing %s (%p)", cp()->pid, r1, prog_addr);
            if (!prog_addr) {
                return -1;
            } else {
                new_proc = knew_proc(prog_addr, exit);
                new_proc->pcb.r0 = 0;
                return new_proc->pid;
                return 0;
            }
        case WAIT:
            printk("Proc %d waiting for proc %d", cp()->pid, r1);
            cp()->wait_pid = r1;
            ksleep_proc(cp());
            return 0;

        case CREATE:
            f = kf_create(r2);
            if (!f)
                return -1;

            kf_add_to_dir((char*)r1, f, cp()->cwd);

            if (f->type == KFS_DIR) {
                kf_add_to_dir(".", f, f);
                kf_add_to_dir("..", cp()->cwd, f);
                return 0;
            } else {
                return kadd_file_proc(cp(), f);
            }
        case OPEN:
            f = kf_lookup((char*)r1, cp()->cwd);
            if (!f) {
                return -1;
            }
            if (f->type == KFS_DIR) {
                return -1;
            }

            return kadd_file_proc(cp(), f);
        case CLOSE:
            if (r1 == 0 || r1 >= NUM_FDS)
                return -1;
            return kclose_file_proc(cp(), r1);
        case WRITE:
            if (r1 == 0 || r1 >= NUM_FDS)
                return -1;

            f = cp()->files[r1].file;
            if (!f)
                return -1;

            ret = kf_write(f, (char*)r2, r3, cp()->files[r1].pos);
            if (ret > 0)
                cp()->files[r1].pos += ret;
            return ret;
        case READ:
            if (r1 == 0 || r1 >= NUM_FDS)
                return -1;

            f = cp()->files[r1].file;
            if (!f)
                return -1;

            ret = kf_read(f, (char*)r2, r3, cp()->files[r1].pos);
            if (ret > 0)
                cp()->files[r1].pos += ret;
            return ret;

        case GET_DIR_ENTRIES:
            return kf_get_dir_entries(cp()->cwd, (void*)r1, r2);

        case GET_CWD:
            strlcpy((char*)r1, cp()->cwd->dir_name, r2);
            return 0;

        case SET_CWD:
            f = kf_lookup((char*)r1, cp()->cwd);
            if (!f || f->type != KFS_DIR) {
                return -1;
            }
            cp()->cwd = f;
            return 0;

        case SEEK:
            if (r1 == 0 || r1 >= NUM_FDS)
                return -1;

            switch (r3) {
                case SEEK_ABS:
                    cp()->files[r1].pos = r2;
                    return 0;
                case SEEK_REL:
                    cp()->files[r1].pos += r2;
                    return 0;
                default:
                    return -1;
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
    // ret == -255 means that the process doesn't exist anymore
    // or something else went wildly wrong
    if (ret != -255) {
        pcb->r0 = ret;
    }
    proc *p = ksched();
    return &p->pcb;
}
