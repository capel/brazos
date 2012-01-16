
#define KERNEL


#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "kexec.h"
#include "sys/ko.h"
#include "task.h"

dir* mk_fs(void);
dir* mk_proc(void);
ko* mk_sched(void);
dir* mk_queue(void);

static dir* _root;
dir* root(void) {
  return _root;
}

static sinkhole* _raw_stdin;
sinkhole* raw_stdin() {
  return _raw_stdin;
}

void setup(void) {
  printk("setup");
  setup_sched();
  printk("after sched");
  setup_ko_registry();
  printk("done with registry");
  setup_err_ko();

  _root = mk_dir();
  printk("root %k", _root);
  KO(root())->rc = 99;

  SAFE_ADD(root(), mk_fs(), "fs");

  dir* stdin = mk_queue();
  _raw_stdin = SINKHOLE(LOOKUP(stdin, "push"));
  kget(_raw_stdin);
  assert(IS_SINKHOLE(_raw_stdin));

  fountain* f = FOUNTAIN(LOOKUP(stdin, "pop"));
  assert(IS_FOUNTAIN(f));
  printk("linked %k", f);

  LINK(root(), f, "stdin");

  
  dir* proc = mk_dir();
  LINK(root(), proc, "proc");
  //SAFE_ADD(proc, mk_ctor(proc_me, KO_DIR), "me");
  printk("proc %k", proc);
  assert(IS_DIR(proc));
  kput(proc);

  printk("done");

  //SAFE_ADD(root(), mk_sched(), "sch");

}

void setup_irq(void);

void kmain(void) {
  ksetup_memory();
  reset_kernel_vm();

  //enable_mmu();

  char* irq_stack = kget_pages(10);
  set_irq_stack(irq_stack+10*PAGE_SIZE, 0,0,0);


  setup();

  enable_irq(0);
 // enable_rtc_irq();
//  set_timer(100);
  enable_cons_irq();

  task* t = mk_user_task(sh_main);
  enqueue_task(t);
  sched();
}

