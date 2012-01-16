#ifndef SCHED_H
#define SCHED_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"
#include "sys/ko.h"
#include "sys/idir.h"
#include "malloc.h"

typedef struct _proc {
    dir* ko;
    idir* rids;
} proc;

int proc_add_ko(proc* p, ko* o);
ko* proc_rid(proc* p, int rid);

proc * knew_proc(void* main, void* exit);
void kfree_proc(proc* p);

void kexec_proc(proc *p, void* main, void* exit);

proc * cp(void);


#endif
