#ifndef SCHED_H
#define SCHED_H

#include "common.h"
#include "proc.h"

typedef ent proc;

void restore_pcb(PCB* pcb);

ent* kcreate_sched(void);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);

proc * cp(void);

#endif
