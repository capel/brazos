#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"
#include "kexec.h"
#include "bcache.h"
#include "sys/dir.h"

ko* mk_dir(void);
ko* mk_fs(void);
ko* mk_proc(void);
ko* mk_sched(void);
ko* mk_queue(void);

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;

static ko* _new_root;
ko* new_root(void) {
  return _new_root;
}

PCB* kirq (PCB* stacked_pcb) {
  newchar = true;
  return stacked_pcb;
}

void null_ptr_func() {
  panic("NULL PTR FUNC HIT.");
}

int update_input() {
  int c = kgetc();
  switch(c) {
  case '\r':
    c = '\n';
    kputc(c);
    return c;
    break;
  case '\b':
    /*
        kputc('\b');
        kputc(' ');
        kputc('\b');
    */
    return '\b';
  case 27:
    c = kgetc();
    if (c == 91) {
      switch (kgetc()) {
      case 'D':
        return ARROW_LEFT;
      case 'C':
        return ARROW_RIGHT;
      case 'A':
        return ARROW_UP;
      case 'B':
        return ARROW_DOWN;
      default :
        return BAD_CODE;
      }
    } else {
      return BAD_CODE;
    }
  default:
    break;
  }

  kputc(c);

  /*if (!isalpha(c) && !isdigit(c)) {
      printk("<%d>", c);
  }*/

  return c;

}

static ko* proc_me(void * ignore) {
  return cp()->ko;
}

void setup(void) {
  ksetup_sched();

  _new_root = mk_dir();
  printk("root %k", _new_root);
  SAFE_ADD(new_root(), mk_fs(), "fs");

  ko* proc = mk_dir();
  LINK(new_root(), proc, "proc");
  SAFE_ADD(proc, bind0(proc_me, 0), "me");
  printk("%h", ((dir*)proc)->h);
  kput(proc);

  SAFE_ADD(new_root(), mk_sched(), "sch");

}

void kmain(void) {
  printk("Starting main");
  enable_interrupt();
  enable_cons_irq();

  //enable_mmu();

  ksetup_memory();

  printk("setup mem done");

  char* swi_stack = kget_pages(10);
  set_swi_stack(swi_stack+10*PAGE_SIZE, 0,0,0);

  setup();

  memset(input, 0, 500);
  inputpos = 0;

  knew_proc(sh_main, exit);
  //knew_proc(main2, exit);
  proc *p = ksched();
  p->parent_pid = p->pid;

  // kflush_file(root());
  restore_pcb(&p->pcb);
}

char* parent_path(const char* path) {
  size_t size = strlen(path)+1;
  char* newpath;
  for(size_t i = 0; i < size; i++) {
    if (path[i] == '/') {
      goto complex_path;
    }
  }
  // the path isn't complex (read: is just the filename)
  // so we just return "." for the current directory.
  return ".";

complex_path:

  newpath = kmalloc(size);
  strlcpy(newpath, path, size);
  for(int i = size; i >= 0; i--) {
    if (newpath[i] == '/') {
      newpath[i] = '\0'; // null it out to end the path
      return newpath;
    }
  }
  return newpath;
}


static int sys_getc(void) {
  for (;;) {
    int c = update_input();
    if (c) return c;
  }
}


static int sys_putc(int c) {
  kputc(c);
  return 0;
}
static int sys_halt(void) {
  khalt();
  return 0;
}
static int sys_write_stdout(const char* s) {
  kputs(s);
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


static int sys_wait(int pid) {
  printk("Proc %d waiting for proc %d", cp()->pid, pid);
  proc * other = proc_by_pid(pid);
  if (!other) return E_BAD_ARG;

  cp()->wait_pid = pid;
  ksleep_proc(cp());
  return 0;
}



static int sys_link(int prid, int crid, const char* name) {
  ko* parent = proc_rid(cp(), prid);
  if (!parent) return E_BAD_FD;
  ko* child = proc_rid(cp(), crid);
  if (!child) return E_BAD_FD;

  return LINK(parent, child, name);
}

static ko* path_lookup(const char* path);

static int sys_lookup(const char* path) {
  ko* o = path_lookup(path);
  if (!o) return E_BAD_FILENAME;

  return proc_add_ko(cp(), o);
}

static ko* path_lookup(const char* path) {
  if (!path || strlen(path) == 0) {
    printk("No path...");
    const char* a[] = {"cwd", 0};
    return LOOKUP(cp()->ko, a);
  }

  if (strcmp(path, "/") == 0) return new_root(); 

  bool abs = (path[0] == '/');

  ko * o;
  vector* v = ksplit_to_vector(abs ? path+1 : path, "/");
  if (abs) {
    o = LOOKUP(new_root(), PATH(v));
  } else {
    const char* a[] = {"cwd", 0};
    ko* cwd = LOOKUP(cp()->ko, a);
    o = LOOKUP(cwd, PATH(v));
  }

  cleanup_vector(v);
  
  return o; 
}


static int sys_map(int rid, size_t* out_size, void** out_ptr) {
  ko* o = proc_rid(cp(), rid);
  if (!o) return E_BAD_FD;

  return MAP(o, out_size, out_ptr);
}


int _ksyscall (int code, int r1, int r2, int r3) {
  switch (code) {
    case PUTC:
      return sys_putc(r1);
    case GETC:
      return sys_getc();
    case WRITE_STDOUT:
      return sys_write_stdout((const char*)r1);
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
      return 0; // sys_unlink((const char*)r1);
    case SYS_LOOKUP:
      return sys_lookup((const char*)r1);
    case SYS_MAP:
      return sys_map(r1, (size_t*)r2, (void**)r3);
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
