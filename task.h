#ifndef TASK_H
#define TASK_H

#include "sys/ko.h"
#include "sys/idir.h"

typedef struct PCB {
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

typedef struct task {
  dir o;
  PCB pcb;
  bool runnable;
  void* stack;
  vm_data *vm;
  dir* d;
  idir* rids;
} task;

void task_wait(task* t);
void task_wake(task* t);

void setup_sched(void);

int proc_add_ko(task* p, ko* o);
ko* proc_rid(task* p, int rid);

void kcopy_pcb(task* t, PCB * pcb);

task* ct(void);


task* mk_user_task(void* start_addr);
task* mk_task(void* start_addr);
void enqueue_task(task* t);

void sched(void);

#endif
