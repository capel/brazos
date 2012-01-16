#include "sched.h"
#include "stdlib.h"
#include "mem.h"
#include "mach.h"
#include "syscalls.h"
#include "sys/ko.h"
#include "task.h"

extern void exit(void);

int proc_add_ko(proc* p, ko* o) {
  if (!o) return 0;
  if (ID(o) < 0) return ID(o);
  idir_insert(p->rids, ID(o), o);
  return ID(o);
}

ko* proc_rid(proc* p, int rid) {
  if (rid < 0) return get_ko(rid);
  return idir_lookup(p->rids, rid);
}

dir* mk_procfs(proc *p);

proc * knew_proc(void* main, void* exit)
{
    proc *p = kmalloc(sizeof(*p));
    p->ko = mk_procfs(p);
    p->rids = mk_idir();

    LINK(p->ko, p->rids, "rids");
    LINK(p->ko, root(), "cwd"); 
    return p;
}

proc * cp() {
    return ct()->user_proc;
}

