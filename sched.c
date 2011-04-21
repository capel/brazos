#include "sched.h"
#include "stdlib.h"
#include "mem.h"
#include "mach.h"

#define PROC_TABLE_SIZE 4


proc *proc_table[PROC_TABLE_SIZE];
size_t proc_table_pos;
size_t cur_pid;
size_t num_procs;
size_t num_runnable_procs;

proc * current;

extern void exit(void);

void kinit_sched()
{
    for (int i = 0; i < PROC_TABLE_SIZE; i++) {
        proc_table[i] = 0;
    }
    
    current = 0;
    cur_pid = 1;
    num_procs = 0;
    num_runnable_procs = 0;
}


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
    p->mem = 0;
    p->runnable = 1;
    p->wait_pid = 0;
    p->cwd = kf_get_root();

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
/*
proc* kfork_proc(proc* p) {
    //printk("%x %x %x %x %x %x %x %x %x %x", p->pcb.sp,
 //       p->pcb.r12, p->pcb.fp, p->pcb.r10, p->pcb.r9, p->pcb.r8, p->pcb.r7, p->pcb.r6, p->pcb.r5, p->pcb.r4);
    proc *p2 = knew_proc(p->pcb.pc, exit);
    memcpy((char*)&p2->pcb, (char*)&p->pcb, sizeof(PCB));
    p2->pcb.pc = p->pcb.pc;
    p2->mem = p->mem;
    memcpy(p2->stack, p->stack, USER_STACK_SIZE * PAGE_SIZE);

    size_t diff = p->stack - p->pcb.sp;
    size_t diff2 = (void*)p->pcb.fp - p->pcb.sp;
    p2->pcb.sp = p2->stack + diff;
    p2->pcb.fp = p2->pcb.sp + diff2;
    //printk("p2 %p : diff %d : pc %p, sp %p", p2, diff, p2->pcb.pc, p2->pcb.sp);
    return p2;
}
*/
void kexec_proc(proc* p, void* main, void* exit) {
    p->pcb.sp = p->stack;
    p->pcb.lr = exit;
    p->pcb.pc = main;
/*
    // free mem they can't access anymore. 
    for(proc_pages* mem = p->mem; mem; ) {
        proc_pages* tmp;
        kfree_pages(mem->pages, mem->num_pages);
        tmp = mem;
        mem = mem->next;
        kfree(tmp);
*/
        p->mem = 0;
//    }
}

void kwake_procs(int pid) {
    for(size_t i = 0; i < PROC_TABLE_SIZE; i++) {
        proc *p = proc_table[i];
        if (p && p->wait_pid == pid) {
            kwake_proc(p);
        }
    }
}

int kadd_file_proc(proc * p, kfile * f) {
    for(size_t i = 0; i < NUM_FDS; i++) {
        if (p->files[i].file == 0) {
            p->files[i].file = f;
            p->files[i].pos = 0;
            return i;
        }
    }
    return -1;
}

int kclose_file_proc(proc *p, int fd) {
    p->files[fd].file = 0;
    p->files[fd].pos = 0;
    return 0;
}

void kfree_proc(proc *p)
{
    for (size_t i = 0; i < PROC_TABLE_SIZE; i++) {
        if (p == proc_table[i]) {
            proc_table[i] = 0;
            num_procs--;
            kwake_procs(p->pid);
            
            // file cleanup code, eventually
/*           
            for(proc_pages* mem = p->mem; mem; ) {
                proc_pages* tmp;
                kfree_pages(mem->pages, mem->num_pages);
                tmp = mem;
                mem = mem->next;
                kfree(tmp);
            }
*/         
            kfree_pages(p->stack, USER_STACK_SIZE);
            kfree(p);
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
    if (num_runnable_procs == 0) {
        panic("Sched with no runnable procs!!!!");
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

void* kget_pages_for_user(proc* p, size_t num)
{
    void* pages = kget_pages(num);
    proc_pages * pp = kmalloc(sizeof(*pp));
    pp->pages = pages;
    pp->num_pages = num;
    pp->next = p->mem;
    p->mem = pp;
    return pages;
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
