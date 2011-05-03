#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"
#include "kfs.h"

#define NUM_FDS 4


typedef struct _PCB {
    int spsr;
    int r0;  
    int r1;
    int r2;
    int r3;
    int r4;
    int r5;
    int r6;
    int r7;
    int r8;
    int r9;
    int r10;
    int fp;
    int r12;
    void* sp;
    void* lr;
    void* pc;
} PCB;

typedef struct _proc_pages {
    void* pages;
    size_t num_pages;
    struct _proc_pages * next;
} proc_pages;

typedef struct _proc_file {
    kfile* file;
    size_t pos;
} proc_file;

typedef struct _proc {
    PCB pcb;
    size_t stride;
    bool runnable;
    int pid;
    int wait_pid;
    proc_pages * mem;
    void* stack;
    kfile* cwd;
    proc_file files[NUM_FDS];
} proc;

void restore_pcb(PCB* pcb);


void ksetup_sched(void);

proc * knew_proc(void* main, void* exit);
void kfree_proc(proc* p);

proc* kfork_proc(proc* p);
void kexec_proc(proc *p, void* main, void* exit);

void ksleep_proc(proc *p);
void kwake_proc(proc *p);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);

int kadd_file_proc(proc * p, kfile * f);
int kclose_file_proc(proc *p, int fd);

void* kget_pages_for_user(proc* p, size_t num);

proc * ksched(void);



proc * proc_by_pid(int pid);
proc * cp(void);

#endif
