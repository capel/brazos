#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"
#include "kfs.h"

void restore_pcb(PCB* pcb);

void ksetup_sched(void);

proc * kcreate_proc(void* main, void* exit);

proc* kfork_proc(proc* p);
void kexec_proc(proc *p, void* main, void* exit);

// implicitly the current proc
void kcopy_pcb(PCB * pcb);

proc * ksched(void);

proc * proc_by_pos(size_t pos);

kfile* kget_procfile(proc *p);


proc * proc_by_pid(int pid);
proc * cp(void);

#endif
