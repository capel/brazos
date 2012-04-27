#include "../common/syscalls.h"
#include "include/common.h"
#include "include/proc.h"
#include "include/sched.h"

int update_input(void);

static bool newchar;
PCB* kirq (PCB* stacked_pcb) {
    newchar = true;
    return stacked_pcb;
}

void null_ptr_func() {
    panic("NULL PTR FUNC HIT.");
}

int _ksyscall (int code, int r1, int r2, int r3) 
{
  const char* path = "UNINIT";
  ent* e = NULL;
  err_t err; 

    switch (code) {
        case SYS_PUTC:
            kputc(r1);
            return 0;
        case SYS_GETC:
            for (;;) {
                int c = update_input();
                if (c) return c;
            }
        case SYS_WRITE_STDOUT:
            kputs((char*)r1);
            return 0;
        case SYS_YIELD:
            return 0;
        case SYS_LOOKUP:
            path = (const char*)r1;
            if (!path) return E_BAD_ARG;
            if (path[0] == '/') {
              e = FS(path);
              if (IS_ERR(e)) return ERR(e);
            } else {
              e = FS("/proc/me/cwd/%s", path);
              if (IS_ERR(e)) return ERR(e);
            }
            rid_t rid = (rid_t) FS("/proc/me/rid/new!");
            if (IS_ERR(rid)) return rid;
            err = LINK_FS(e, "/proc/me/rid/%d", rid);
            if (!OK(err)) return err;
            return rid;
            
        default:
            printk("Bad syscall code %d", code);
            return E_BAD_SYSCALL;
    }
}

PCB* ksyscall (void* stacked_pcb) {
    kcopy_pcb(stacked_pcb);
    PCB* pcb = &PROC_PCB(FS("/proc/me"));
    int ret = _ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
    // ret == -255 means that the process doesn't exist anymore
    // or something else went wildly wrong
    if (ret != -255) {
        pcb->r0 = ret;
    }
    proc *p = FS("/proc/sched!");
    return &PROC_PCB(p);
}
