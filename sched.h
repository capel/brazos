#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"
#include "sys/ko.h"
#include "sys/kihashmap.h"

#define NUM_FDS 4
#define PROC_TABLE_SIZE 4
#define NUM_KOS 8

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

typedef struct _proc {
    PCB pcb;
    bool runnable;
    int stride;
    int parent_pid;
    int pid;
    void* stack;
    void* next_future;
    dir* ko;
    kihashmap* rids;
} proc;

int proc_add_ko(proc* p, ko* o);
ko* proc_rid(proc* p, int rid);

void restore_pcb(PCB* pcb);

void ksetup_sched(void);

proc * knew_proc(void* main, void* exit);
void kfree_proc(proc* p);

proc* kfork_proc(proc* p);
void kexec_proc(proc *p, void* main, void* exit);

//void ksleep_proc(proc *p, future* f);
//void kwake_proc(proc *p, future* f);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);


void* kget_pages_for_user(proc* p, size_t num);

proc * ksched(void);

proc * proc_by_pos(size_t pos);

proc * proc_by_pid(int pid);
proc * cp(void);

#endif
