#include "sched.h"
#include "stdlib.h"
#include "mem.h"
#include "mach.h"
#include "kfs.h"
#include "procfs.h"
#include "syscalls.h"

proc *proc_table[PROC_TABLE_SIZE];
size_t proc_table_pos;
size_t cur_pid;
size_t num_procs;
size_t num_runnable_procs;

proc * current;

proc * proc_by_pos(size_t pos) {
    return proc_table[pos];
}

extern void exit(void);

ko* new_root(void);

void ksetup_sched()
{
    for (int i = 0; i < PROC_TABLE_SIZE; i++) {
        proc_table[i] = 0;
    }
    
    current = 0;
    cur_pid = 1;
    num_procs = 0;
    num_runnable_procs = 0;
}

int proc_add_ko(proc* p, ko* o) {
  int rid = p->current_rid++;
  kihm_insert(p->rids, rid, o);
  return rid;
}

ko* proc_rid(proc* p, int rid) {
  return kihm_lookup(p->rids, rid);
}

ko* mk_procfs(proc *p);

proc * knew_proc(void* main, void* exit)
{
    if (num_procs > PROC_TABLE_SIZE) {
        printk("No space in proc table. Abort knew_proc");
        return 0;
    }

    proc *p = kmalloc(sizeof(*p));
    assert(p);
    printk("%p", p);
    p->pid = cur_pid++;
    p->stride = 0;
    p->runnable = 1;
    p->wait_pid = 0;
    p->ko = mk_procfs(p);
    p->rids = mk_kihashmap(4);
    p->current_rid = 1;

    LINK(p->ko, new_root(), "cwd"); 
    
    const char* b[] = {"proc", 0};
    ko* procdir = LOOKUP(new_root(), b); 

    char n[32];
    itoa(n, 32, p->pid);
    LINK(procdir, p->ko, n); // dont kput because we have a reference

    const char* c[] = {"sch", "runnable", 0};
    ko* runnable = LOOKUP(new_root(), c); 
    LINK(runnable, p->ko, "push@");

    unsigned spsr = __get_CPSR();
    p->pcb.spsr = spsr & (~0x1f);
    p->pcb.spsr |= 0x10;
    p->pcb.pc = main;
    p->pcb.lr = exit;
    p->stack = kget_pages(USER_STACK_SIZE) + USER_STACK_SIZE * PAGE_SIZE;
    p->pcb.sp = p->stack;

    // look for table gaps
    // we know there is enough space
    for(size_t i = 0; i < PROC_TABLE_SIZE; i++)
    {
        if (proc_table[i] == 0) {
            proc_table[i] = p;
            num_procs++;
            num_runnable_procs++;
            printk("New proc %d at slot %d", p->pid, i);
            return p;
        }
    }
    // bad!
    panic("No free procs after initial check said there were");
}

void kexec_proc(proc* p, void* main, void* exit) {
    p->pcb.sp = p->stack;
    p->pcb.lr = exit;
    p->pcb.pc = main;
}

void kwake_procs(int pid) {
  printk("in wake for pid %d", pid);
    for(size_t i = 0; i < PROC_TABLE_SIZE; i++) {
        proc *p = proc_table[i];
        if (p && p->wait_pid == pid) {
            kwake_proc(p);
        }
    }
}


void kfree_proc(proc *p)
{
    for (size_t i = 0; i < PROC_TABLE_SIZE; i++) {
        if (p == proc_table[i]) {
            proc_table[i] = 0;
            num_procs--;
            num_runnable_procs--;
            kwake_procs(p->pid);
            
            kfree(p);
            return;
        }
    }
}

void ksleep_proc(proc* p) {
    num_runnable_procs--;
    p->runnable = 0;
}

void kwake_proc(proc* p) {
  printk("waking proc %d", p->pid);
    num_runnable_procs++;
    p->runnable = 1;
}

void kcopy_pcb(PCB *pcb) {
    if (&current->pcb == pcb) {
        printk("current and src are the same [%p]?", pcb);
    }
    memcpy((char*)&current->pcb, (char*) pcb, sizeof(PCB));
}

proc* ksched(void)
{
    if (num_runnable_procs == 0) {
        panic("No procs. Goodbye world!");
    }

    select:

    for(int i = 0; i < PROC_TABLE_SIZE; i++) {
        proc *p = proc_table[i];
        if (p && p->runnable && p->stride == 0) {
            p->stride = 1;
            current = p;
            return p;
        }
    }

    // everyone has been tested, nobody is left
    for (int i = 0; i < PROC_TABLE_SIZE; i++) {
        if (proc_table[i]) {
            proc_table[i]->stride = 0;
        }
    }

    // we know this won't loop more than one time
    // because num_procs says there is one to run
    // and we just enabled every proc
    goto select;
}

proc* proc_by_pid(int pid) {
    for (int i = 0; i < PROC_TABLE_SIZE; i++) {
        proc *p = proc_table[i];
        if (p && p->pid == pid)
            return p;
    }
    printk("Can't find proc for pid %d", pid);
    return 0;
}

proc * cp(void) {
    return current;
}
