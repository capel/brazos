#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"
#include "kexec.h"
#include "sys/ko.h"


static int sys_getc(void) {
  for (;;) {
    //int c = update_input();
    //if (c) return c;
  }
  return 0;
}


static int sys_putc(int c) {
  kputc(c);
  return 0;
}
static int sys_halt(void) {
  khalt();
  return 0;
}

static int sys_exit(void) {
  printk("Proc %d exiting", cp()->pid);
  kfree_proc(cp());
  return -255;
}

static int sys_exec(const char * prog_name) {
  void* prog_addr = program_lookup(prog_name);
  printk("Proc %d execing %s (%p)", cp()->pid, prog_name, prog_addr);
  if (!prog_addr) return E_BAD_PROGRAM;

  kexec_proc(cp(), prog_addr, exit);
  return 0;
}



static int sys_forkexec(const char* prog_name) {
  void* prog_addr = program_lookup(prog_name);
  printk("Proc %d execing %s (%p)", cp()->pid, prog_name, prog_addr);
  if (!prog_addr) return E_BAD_PROGRAM;

  proc* new_proc = knew_proc(prog_addr, exit);
  new_proc->parent_pid = cp()->pid;
  new_proc->pcb.r0 = 0;
  return new_proc->pid;
}


static int sys_wait(int rid) {
  ko *o = proc_rid(cp(), rid);
  if (!o) return E_BAD_RID;
  if (IS_RESOLVED(o)) return proc_add_ko(cp(), GET_FUTURE(FUTURE(o)));

  if (IS_FUTURE(o)) return E_ERROR; // do something

  return rid; // wait on non-future is a yield basicly
}


static int sys_link(int prid, int crid, const char* name) {
  dir* parent = (dir*)proc_rid(cp(), prid);
  if (!parent) return E_BAD_RID;
  if (!IS_DIR(parent)) return E_NOT_DIR;

  ko* child = proc_rid(cp(), crid);
  if (!child) return E_BAD_RID;

  return proc_add_ko(cp(), LINK(parent, child, name));
}

static int sys_unlink(int prid, const char* name) {
  dir* parent = DIR(proc_rid(cp(), prid));
  if (!parent) return E_BAD_RID;
  if (!IS_DIR(parent)) return E_NOT_DIR;
  if (!name) return E_BAD_ARG;
  return proc_add_ko(cp(), UNLINK(parent, name));
}


static ko* path_lookup(const char* path);

static int sys_lookup(const char* path) {
  ko* o = path_lookup(path);
  if (!o) return E_BAD_FILENAME;

  return proc_add_ko(cp(), o);
}

static int sys_dredge(int rid) {
  ko* o = proc_rid(cp(), rid);
  if (!o) return E_BAD_RID;
  if (!IS_FOUNTAIN(o)) return E_NOT_FOUNTAIN;

  return proc_add_ko(cp(), DREDGE(FOUNTAIN(o)));
}

static ko* path_lookup(const char* path) {
  if (!path || strlen(path) == 0) {
    ko* o = walk(cp()->ko, "cwd");
    return o;
  }

  if (strcmp(path, "/") == 0) return KO(new_root()); 
  if (strcmp(path, "~") == 0) return KO(cp()->ko);

  if (path[0] == '/') {
    return walk(new_root(), path+1);
  } else if (path[0] == '~') {
    return walk(cp()->ko, path+1);
  } else {
    ko* cwd = walk(cp()->ko, "cwd");
    assert(IS_DIR(cwd));
    return walk(DIR(cwd), path);
  }
}

static int sys_type(int rid) {
  ko* o = proc_rid(cp(), rid);
  if (!o) return E_BAD_RID;
  return o->type;
}

static int sys_sink(int src_rid, int sh_rid) {
  ko* src = proc_rid(cp(), src_rid);
  ko* sh = proc_rid(cp(), sh_rid);
  if (!sh || !src) {
    return E_BAD_RID;
  }
  if (!IS_SINKHOLE(sh)) {
    return E_NOT_SINKHOLE;
  }
  ko* ret = SINK(SINKHOLE(sh), src);
  if (!ret) return SINK_ASYNC;

  return proc_add_ko(cp(), ret);
}

static int sys_message(void* data, size_t size) {
  if (size == 0 || data == 0) {
    return E_BAD_ARG;
  }
  ko* f = KO(mk_msg(data));
  return proc_add_ko(cp(), KO(f));
}

static int sys_view(int rid, char* buf, size_t len) {
  ko* o = proc_rid(cp(), rid);
  if (!o) return 0;
  msg * m = VIEW(o);
  strlcpy(buf, get_msg(m), len);
  kput(m);
  return 0;
}

int _ksyscall (int code, int r1, int r2, int r3) {
  switch (code) {
    case PUTC:
      return sys_putc(r1);
    case GETC:
      return sys_getc();
    case HALT:
      return sys_halt();
    case EXIT:
      return sys_exit();
    case YIELD:
      return 0;
    case EXEC:
      return sys_exec((const char*)r1);
    case FORKEXEC:
      return sys_forkexec((const char*)r1);
    case WAIT:
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


PCB* ksyscall (void* stacked_pcb) {
  kcopy_pcb(stacked_pcb);
  PCB* pcb = &cp()->pcb;
  int ret = _ksyscall(pcb->r0, pcb->r1, pcb->r2, pcb->r3);
  // ret == -255 means that the process doesn't exist anymore
  // or something else went wildly wrong
  if (ret != -255) {
    pcb->r0 = ret;
  }
  proc *p = ksched();
  return &p->pcb;
}
