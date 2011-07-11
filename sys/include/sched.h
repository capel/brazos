#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "common.h"
#include "proc.h"

typedef ent proc;
typedef ent sched;

void restore_pcb(PCB* pcb);

sched* ksetup_sched(void);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);

proc * ksched(void);
proc * cp(void);

#endif
