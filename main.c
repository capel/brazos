#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"
#include "kexec.h"
#include "bcache.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

PCB* kirq (PCB* stacked_pcb) {
    newchar = true;
    return stacked_pcb;
}

void null_ptr_func() {
    panic("NULL PTR FUNC HIT.");
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

void setup(void) {
    ksetup_bcache();
    ksetup_disk();
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

    char* swi_stack = kget_pages(10);
    set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);
   
    setup();

    memset(input, 0, 500);
    inputpos = 0;

    knew_proc(sh_main, exit);
    //knew_proc(main2, exit);
    proc *p = ksched();

   // kflush_file(root());
    restore_pcb(&GET_PCB(p->pcb));
}

#define RETURN(r) do { ret = r; goto cleanup; } while (0)

int _ksyscall (int code, int r1, int r2, int r3) 
{
    void* prog_addr = NULL;
    proc* new_proc = NULL;
    int ret;
    ko* o = NULL;
    ko* cwd = NULL;
    string *s = NULL;

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
            if (!prog_addr) return E_BAD_PROGRAM;
            
            kexec_proc(cp(), prog_addr, exit);
            return 0;
        case FORKEXEC:
            prog_addr = program_lookup((char*)r1);
            printk("Proc %d execing %s (%p)", cp()->pid, r1, prog_addr);
            if (!prog_addr) return E_BAD_PROGRAM;

            new_proc = knew_proc(prog_addr, exit);
            new_proc->pcb.r0 = 0;
            return new_proc->pid;
        case WAIT:
            printk("Proc %d waiting for proc %d", cp()->pid, r1);
            cp()->wait_pid = r1;
            ksleep_proc(cp());
            return 0;

        case CREATE:
            if (r2 == CREATE_FILE) {
              o = kcreate_file();
            } else if (r2 == CREATE_DIR) {
              o = kcreate_dir();
            } else {
              RETURN(E_BAD_SYSCALL);
            }
            if (!o) RETURN(E_ERROR);

            ko* cwd = LOOKUP_NAME(cp(), CWD_STRING);
            s = mkstring(r1);
            if (BAD_HANDLE == ADD_CHILD(cwd, s, o)) RETURN(E_NOT_SUPPORTED);

            ADD_PARENT(o, cwd);

            if (r2 == CREATE_DIR) {
              RETURN(SUCCESS);
            } else {
                handle_t h = ADD_CHILD(cp(), FD_STRING, o);
                ADD_PARENT(f, cp());
                RETURN(h);
            }
        case OPEN:
            o = kget_by_path((char*)r1, cp()->cwd);
            if (!o) RETURN(E_BAD_FILENAME);
            
            if (o->type == TYPE_DIR) RETURN(E_IS_DIR);
            printk("F INODE %d", o->inode);

            handle_t h = ADD_CHILD(cp(), o);
            ADD_PARENT(o, cp());
            RETURN(h);
        case CLOSE:
            ko* o = LOOKUP_HANDLE(cp(), r1);
            if (!o) RETURN(E_BAD_FD);

            status_t status = REMOVE_CHILD(cp(), o);
            REMOVE_PARENT(o, cp());
            RETURN(status);
        case WRITE:
            ko* o = LOOKUP_HANDLE(cp(), r1);
            if (!o) RETURN(E_BAD_FD);

            if (!o->f->write) RETURN(E_NOT_SUPPORTED);

            RETURN(WRITE(f, (const char*)r2, r3, r4));
        case READ:
            ko* o = LOOKUP_HANDLE(cp(), r1);
            if (!o) RETURN(E_BAD_FD);

            if (!o->f->read) RETURN(E_NOT_SUPPORTED);

            RETURN(READ(f, (const char*)r2, r3, r4));
        case GET_DIR_ENTRIES:
            return kf_copy_dir_entries(LOOKUP_NAME(cp(), CWD_STRING), (void*)r1, r2);

        case GET_CWD:
            cwd = LOOKUP_NAME(cp(), PWD);
            s = NAME(cwd);
            strlcpy((char*)r1, STRVAL(s), r2);
            RETURN(SUCCESS);

        case SET_CWD:
            o = kget_by_path((char*)r1, cp()->cwd);
            if (!o) RETURN(E_BAD_FILENAME);

            if (o->type != KFS_DIR) RETURN(E_ISNT_DIR); 
            status_t status = ADD_CHILD(cp()->cwd, PWD_STRING, o);
            ADD_PARENT(o, PWD_STRING, cp()->cwd);
            RETURN(status);
        case UNLINK:
            cwd = LOOKUP_NAME(cp(), CWD_STRING);
            f = kget_by_path((char*)r1, cwd);
            if (!f) RETURN(E_BAD_FILENAME);
            if (!cwd) RETURN(E_ERROR);
            
            if (!cwd->f->remove_child) RETURN(E_NOT_SUPPORTED);

            RETURN(REMOVE_CHILD(pwd, o));
        case SEEK:
            assert(false);
        /*    if (r1 == 0 || r1 >= NUM_FDS) return E_BAD_FD;

            switch (r3) {
                case SEEK_ABS:
                    cp()->files[r1].pos = r2;
                    return 0;
                case SEEK_REL:
                    cp()->files[r1].pos += r2;
                    return 0;
                default:
                    return E_BAD_ARG;
            }*/
        default:
            printk("Bad syscall code %d", code);
            return E_BAD_SYSCALL;
    }

    cleanup:
      kput(o);
      kput(cwd);
      kput(proc);
      kput(s);
      return ret;
}

PCB* ksyscall (void* stacked_pcb) {
    kcopy_pcb(stacked_pcb);
    PCB* pcb = &GET_PCB(cp());
    int ret = _ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
    // ret == -255 means that the process doesn't exist anymore
    // or something else went wildly wrong
    if (ret != -255) {
        pcb->r0 = ret;
    }
    proc *p = ksched();
    return &GET_PCB(p->pcb);
}
