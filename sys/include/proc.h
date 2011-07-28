#ifndef PROC_H
#define PROC_H

#include "common.h"

typedef ent proc;

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

typedef struct {
    PCB pcb;
    size_t stride;
    int pid;
    void* stack;
    ent* cwd;
} proc_data;

#define PROC_DATA(p) ((proc_data*)(p)->d1)
#define PROC_STRIDE(p) PROC_DATA(p)->stride
#define PROC_PCB(p) PROC_DATA(p)->pcb

proc* mkproc(size_t pid);

#endif
