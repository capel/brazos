#include "ko.h"
#include "../sched.h"

IGET_FUNC(get_pid, proc, pid);
IGET_FUNC(get_runnable, proc, runnable);
IGET_FUNC(get_parent_pid, proc, parent_pid);

ko* stdio(void *v, ko* sunk) {
  //proc *p = (proc*)v;
  msg * m = VIEW(sunk);
  kputs(get_msg(m));
  kput(m);
  return SINK_ASYNC;
}

dir* mk_procfs(proc *p) {
  dir* d = mk_dir();
  SAFE_ADD(d, mk_window(get_pid, p), "pid");
  SAFE_ADD(d, mk_window(get_runnable, p), "runnable");
  SAFE_ADD(d, mk_window(get_parent_pid, p), "parent");
  SAFE_ADD(d, mk_sinkhole(stdio, p), "stdio");
  return d;
}
