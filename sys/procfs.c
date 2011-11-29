#include "ko.h"
#include "../sched.h"

ko* mk_dir(void);

IGET_FUNC(get_pid, proc, pid);
IGET_FUNC(get_stride, proc, stride);
IGET_FUNC(get_runnable, proc, runnable);
IGET_FUNC(get_wait_pid, proc, wait_pid);
IGET_FUNC(get_parent_pid, proc, parent_pid);

ko* mk_procfs(proc *p) {
  ko* f = mk_dir();
  SAFE_ADD(f, mk_dir(), "rids");
  SAFE_ADD(f, bind0((bound_func)get_pid, p), "pid");
  SAFE_ADD(f, bind0((bound_func)get_runnable, p), "runnable");
  SAFE_ADD(f, bind0((bound_func)get_wait_pid, p), "wait_pid");
  SAFE_ADD(f, bind0((bound_func)get_parent_pid, p), "parent");
  SAFE_ADD(f, bind0((bound_func)get_stride, p), "stride");
  return f;
}
