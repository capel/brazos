#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"

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
    int r11;
    int r12;
    void* sp;
    void* lr;
    void* pc;
} PCB;


typedef struct _proc {
    PCB pcb;
    size_t stride;
    int pid;
} proc;

void restore_pcb(PCB* pcb);


void kinit_sched(void);

proc * knew_proc(void* main, void* exit);
void kfree_proc(proc* p);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);

proc * ksched(void);

proc * cp(void);

#endif