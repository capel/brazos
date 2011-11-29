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

ko* mk_dir(void);
ko* mk_fs(void);
ko* mk_proc(void);

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

void setup(void) {
  ksetup_bcache();
  ksetup_disk();
  ksetup_fs();
  ksetup_sched();

  _new_root = mk_dir();
  SAFE_ADD(new_root(), mk_fs(), "fs");
  SAFE_ADD(new_root(), mk_dir(), "proc");



  SAFE_ADD(new_root(), mk_dir(), "aaaa");
  SAFE_ADD(new_root(), mk_dir(), "bbbb");
  SAFE_ADD(new_root(), mk_dir(), "cccc");
  SAFE_ADD(new_root(), mk_dir(), "dddd");
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

static int sys_get_pages(size_t num) {
  return (int)kget_pages_for_user(cp(), num);
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


static int sys_create(const char * name, int flags) {
  kfile* f = kf_create(flags);
  if (!f) return E_ERROR;

  printk("creating %s", name);

  if (!cp()->cwd->add_file) {
    kput_file(f);
    return E_NOT_SUPPORTED;
  }

  bool success = cp()->cwd->add_file(cp()->cwd, name, f);
  printk("Added %s to cwd? %d", name, success);
  if (!success) {
    kput_file(f);
    return E_ERROR;
  }

  if (f->type == KFS_DIR) {
    printk("Making a dir");
    //char* ppath = parent_path(name);
    f->add_file(f, "..", cp()->cwd);
    return 0;
  } else {
    return kadd_file_proc(cp(), f);
  }
}


static int sys_open(const char* name) {
  kfile* f = kf_lookup(name, cp()->cwd);
  if (!f) return E_BAD_FILENAME;

  if (f->type == KFS_DIR) {
    kput_file(f);
    return E_IS_DIR;
  }
  printk("F INODE %d", f->inode);

  return kadd_file_proc(cp(), f);
}


static int sys_close(int fd) {
  if (fd == 0 || fd >= NUM_FDS) return E_BAD_FD;

  return kclose_file_proc(cp(), fd);
}



static int sys_write(int fd, const char* buf, size_t len) {
  if (fd == 0 || fd >= NUM_FDS) return E_BAD_FD;

  kfile* f = cp()->files[fd].file;

  if (!f) return E_ERROR;
  if (!f->write) return E_NOT_SUPPORTED;

  int ret = f->write(f, buf, len, cp()->files[fd].pos);
  if (ret > 0)
    cp()->files[fd].pos += ret;
  return ret;
}



static int sys_read(int fd, char* buf, size_t len) {
  if (fd == 0 || fd >= NUM_FDS) return E_BAD_FD;

  kfile* f = cp()->files[fd].file;

  if (!f) return E_ERROR;
  if (!f->read) return E_NOT_SUPPORTED;

  int ret = f->read(f, buf, len, cp()->files[fd].pos);
  if (ret > 0) cp()->files[fd].pos += ret;

  return ret;
}



static int sys_get_dir_entries(void* space, size_t size) {
  return kf_copy_dir_entries(cp()->cwd, space, size);
}


static int sys_get_cwd(char* name_space, size_t size) {
  strlcpy((char*)name_space, cp()->cwd->dir_name, size);
  return 0;
}


static int sys_set_cwd(const char* path) {
  kfile* f = kf_lookup(path, cp()->cwd);
  if (!f) return E_BAD_FILENAME;

  if (f->type != KFS_DIR) {
    kput_file(f);
    printk("type: %d", f->type);
    return E_ISNT_DIR;
  }
  cp()->cwd = f;
  return 0;
}

static int sys_link(int prid, int crid, const char* name) {
  ko* parent = proc_rid(cp(), prid);
  if (!parent) return E_BAD_FD;
  ko* child = proc_rid(cp(), crid);
  if (!child) return E_BAD_FD;

  return LINK(parent, child, name);
}

static int sys_unlink(const char* path) {
  kfile * f = kf_lookup(path, cp()->cwd);
  char* ppath = parent_path(path);
  kfile* parent = kf_lookup(ppath, cp()->cwd);
  if (!f) {
    return E_BAD_FILENAME;
  }
  if (!parent) {
    kput_file(f);
    return E_ERROR;
  }
  if (!parent->rm_file) {
    kput_file(f);
    kput_file(parent);
    return E_NOT_SUPPORTED;
  }

  bool success = parent->rm_file(parent, f);
  if (!success) {
    kput_file(f);
    kput_file(parent);
    return E_NOT_SUPPORTED;
  }
  kput_file(f);
  kput_file(parent);
  return 0;
}

static int sys_seek(int fd, int offset, int type) {
  if (fd == 0 || fd >= NUM_FDS) return E_BAD_FD;

  switch (type) {
  case SEEK_ABS:
    cp()->files[fd].pos = offset;
    return 0;
  case SEEK_REL:
    cp()->files[fd].pos += offset;
    return 0;
  default:
    return E_BAD_ARG;
  }
}

static int sys_lookup(const char* path) {
  printk("in lookup with path %s", path);
  if (!path || strlen(path) == 0) {
    printk("No path...");
    path = "/";
  }
  bool abs = (path[0] == '/');

  ko * o;
  vector* v = ksplit_to_vector(path, "/");
  printk("vector %p size %d", v, v->size);
  printk("new root %p", new_root());
  print_vector(v, "%s", 0);
  if (abs) {
    o = LOOKUP(new_root(), (const char**)v->data);
  } else {
    o = LOOKUP(cp()->cwd_ko, (const char**)v->data);
  }

  cleanup_vector(v);

  printk("o %p", o);
  if (!o) return E_BAD_FILENAME;

  return proc_add_ko(cp(), o);
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
    case GET_PAGES:
      return sys_get_pages((int)r1);
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
    case CREATE:
      return sys_create((const char*)r1, r2);
    case OPEN:
      return sys_open((const char*)r1);
    case CLOSE:
      return sys_close(r1);
    case SYS_LINK:
      return sys_link(r1, r2, (const char*)r3);
    case WRITE:
      return sys_write(r1, (const char*)r2, r3);
    case READ:
      return sys_read(r1, (char*)r2, r3);
    case GET_DIR_ENTRIES:
      return sys_get_dir_entries((void*)r1, r2);
    case GET_CWD:
      return sys_get_cwd((char*)r1, r2);
    case SET_CWD:
      return sys_set_cwd((const char*)r1);
    case SYS_UNLINK:
      return sys_unlink((const char*)r1);
    case SEEK:
      return sys_seek(r1, r2, r3);
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
