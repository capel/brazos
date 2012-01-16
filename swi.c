#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "kexec.h"
#include "task.h"
#include "sys/ko.h"

static int sys_halt(void) {
  khalt();
  return 0;
}


static ko* wake_future_sleep(void* v, ko* o) {
  task *t = (task*)v;
  t->pcb.r0 = proc_add_ko(ct(), o);
  task_wake(t);
  return SINK_ASYNC;
}


static int sys_wait(int rid) {
  if (rid == 0) return 0; // basicly a yield
  ko *o = proc_rid(ct(), rid);
  if (!o) return E_BAD_RID;
  if (!IS_FUTURE(o)) return rid; // wait on non-future is a no op
  if (IS_RESOLVED(o)) return proc_add_ko(ct(), GET_FUTURE(FUTURE(o)));

  // we now have a non-resolved future. Let's put it to sleep.
  future * f = FUTURE(o);
  sinkhole* wake = mk_sinkhole(wake_future_sleep, ct());
  set_listener(f, wake);
  task_wait(ct());
  return 0;
}


static int sys_link(int prid, int crid, const char* name) {
  dir* parent = (dir*)proc_rid(ct(), prid);
  if (!parent) return E_BAD_RID;
  if (!IS_DIR(parent)) return E_NOT_DIR;

  ko* child = proc_rid(ct(), crid);
  if (!child) return E_BAD_RID;

  return proc_add_ko(ct(), LINK(parent, child, name));
}

static int sys_unlink(int prid, const char* name) {
  dir* parent = DIR(proc_rid(ct(), prid));
  if (!parent) return E_BAD_RID;
  if (!IS_DIR(parent)) return E_NOT_DIR;
  if (!name) return E_BAD_ARG;
  return proc_add_ko(ct(), UNLINK(parent, name));
}


static ko* path_lookup(const char* path);

static int sys_lookup(const char* path) {
  ko* o = path_lookup(path);
  if (!o) return E_BAD_FILENAME;

  return proc_add_ko(ct(), o);
}

static int sys_dredge(int rid) {
  ko* o = proc_rid(ct(), rid);
  if (!o) return E_BAD_RID;
  if (!IS_FOUNTAIN(o)) return E_NOT_FOUNTAIN;

  return proc_add_ko(ct(), DREDGE(FOUNTAIN(o)));
}

static ko* path_lookup(const char* path) {
  if (!path || strlen(path) == 0) {
    ko* o = walk(DIR(ct()), "cwd");
    return o;
  }

  if (strcmp(path, "/") == 0) return KO(root()); 
  if (strcmp(path, "~") == 0) return KO(ct());

  if (path[0] == '/') {
    return walk(root(), path+1);
  } else if (path[0] == '~') {
    return walk(DIR(ct()), path+1);
  } else {
    ko* cwd = walk(DIR(ct()), "cwd");
    assert(IS_DIR(cwd));
    return walk(DIR(cwd), path);
  }
}

static int sys_type(int rid) {
  ko* o = proc_rid(ct(), rid);
  if (!o) return E_BAD_RID;
  return o->type;
}

static int sys_sink(int src_rid, int sh_rid) {
  ko* src = proc_rid(ct(), src_rid);
  ko* sh = proc_rid(ct(), sh_rid);
  if (!sh || !src) {
    return E_BAD_RID;
  }
  if (!IS_SINKHOLE(sh)) {
    return E_NOT_SINKHOLE;
  }
  ko* ret = SINK(SINKHOLE(sh), src);
  if (!ret) return SINK_ASYNC;

  return proc_add_ko(ct(), ret);
}

static int sys_message(void* data, size_t size) {
  if (size == 0 || data == 0) {
    return E_BAD_ARG;
  }
  ko* f = KO(mk_msg(data));
  return proc_add_ko(ct(), KO(f));
}

static int sys_view(int rid, char* buf, size_t len) {
  ko* o = proc_rid(ct(), rid);
  if (!o) return 0;
  msg * m = VIEW(o);
  strlcpy(buf, get_msg(m), len);
  kput(m);
  return 0;
}

static int dispatch (int code, int r1, int r2, int r3) {
  switch (code) {
    case HALT:
      return sys_halt();
    case SYS_WAIT:
      return sys_wait(r1);
    case SYS_LINK:
      return sys_link(r1, r2, (const char*)r3);
    case SYS_UNLINK:
      return sys_unlink(r1, (const char*)r2);
    case SYS_LOOKUP:
      return sys_lookup((const char*)r1);
    case SYS_SINK:
      return sys_sink(r1, r2);
    case SYS_MESSAGE:
      return sys_message((void*)r1, (size_t)r2);
    case SYS_TYPE:
      return sys_type(r1);
    case SYS_VIEW:
      return sys_view(r1, (char*)r2, r3);
    case SYS_DREDGE:
      return sys_dredge(r1);
    default:
      printk("Bad syscall code %d", code);
      return E_BAD_SYSCALL;
  }
}

void _sched();

void ksyscall (void* stacked_pcb) {
  kcopy_pcb(ct(), stacked_pcb);
  PCB* pcb = &ct()->pcb;
  reset_kernel_vm();
  pcb->r0 = dispatch(pcb->r0, pcb->r1, pcb->r2, pcb->r3);

  enqueue_task(ct());
  _sched();
}
