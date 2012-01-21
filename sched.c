#include "sched.h"
#include "stdlib.h"
#include "mem.h"
#include "mach.h"
#include "syscalls.h"
#include "sys/ko.h"

static proc *proc_table[PROC_TABLE_SIZE];
//static size_t proc_table_pos;
static size_t cur_pid;
static size_t num_procs;
static size_t num_runnable_procs;

static proc * current;

proc * proc_by_pos(size_t pos) {
    return proc_table[pos];
}

extern void exit(void);

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
  if (!o) return 0;
  if (ID(o) < 0) return ID(o);
  idir_insert(p->rids, ID(o), o);
  return ID(o);
}

ko* proc_rid(proc* p, int rid) {
  if (rid < 0) return get_ko(rid);
  return idir_lookup(p->rids, rid);
}

dir* mk_procfs(proc *p);

proc * knew_proc(void* main, void* exit)
{
    if (num_procs > PROC_TABLE_SIZE) {
        printk("No space in proc table. Abort knew_proc");
        return 0;
    }

    proc *p = kmalloc(sizeof(*p));
    assert(p);
    p->pid = cur_pid++;
    p->stride = 0;
    p->runnable = 1;
    p->ko = mk_procfs(p);
    p->rids = mk_idir();
    printk("rids %p, %k", p->rids, p->rids);
    printk("proc %p", p);

    LINK(p->ko, p->rids, "rids");
    LINK(p->ko, root(), "cwd"); 
    
    dir* procdir = DIR(LOOKUP(root(), "proc"));
    printk("dir %k");
    assert(IS_DIR(procdir));

    char n[32];
    itoa(n, 32, p->pid);
    LINK(DIR(procdir), p->ko, n); // dont kput because we have a reference

    /*
    const char* c[] = {"sch", "runnable", "push@", 0};
    sinkhole* runnable = SINKHOLE(LOOKUP(new_root(), c));
    assert(IS_SINKHOLE(runnable));
    SINK(runnable, p->ko);
    */

    unsigned spsr = __get_CPSR();
    p->pcb.spsr = spsr & (~0x1f);
    p->pcb.spsr |= 0x10;
    p->pcb.pc = main;
    p->pcb.lr = exit;
    p->stack = kget_pages(USER_STACK_SIZE) + USER_STACK_SIZE * PAGE_SIZE;
    p->pcb.sp = p->stack;

    p->vm = kmalloc(sizeof(vm_data));
    memset((void*)p->vm, 0, sizeof(vm_data));

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

void kfree_proc(proc *p)
{
    for (size_t i = 0; i < PROC_TABLE_SIZE; i++) {
        if (p == proc_table[i]) {
            proc_table[i] = 0;
            num_procs--;
            num_runnable_procs--;
            
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
    while (num_runnable_procs == 0) {
      // wait for interrupt
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

proc * cp() {
    return current;
}

static vm_data kvm_data;

void reset_kernel_vm(void) {
  set_vm_base(&kvm_data);
}
