#include "ko.h"
#include "../sched.h"

IGET_FUNC(get_pid, proc, pid);
IGET_FUNC(get_runnable, proc, runnable);
IGET_FUNC(get_parent_pid, proc, parent_pid);

ko* stdio(proc *p, ko* sunk) {
  size_t size;
  void* ptr;
  assert(IS_FILE(sunk));
  MAP(FILE(sunk), &size, &ptr);

  kputs(ptr);
  return SINK_ASYNC;
}

dir* mk_procfs(proc *p) {
  dir* d = mk_dir();
  SAFE_ADD(d, mk_dir(), "rids");
  SAFE_ADD(d, BIND(get_pid, p), "pid");
  SAFE_ADD(d, BIND(get_runnable, p), "runnable");
  SAFE_ADD(d, BIND(get_wait_pid, p), "wait_pid");
  SAFE_ADD(d, BIND(get_parent_pid, p), "parent");
  SAFE_ADD(d, BIND(get_stride, p), "stride");
  SAFE_ADD(d, MK_SINKHOLE(stdio, p), "stdio");
  return d;
}
