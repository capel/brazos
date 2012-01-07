
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

dir* mk_fs(void);
dir* mk_proc(void);
ko* mk_sched(void);
dir* mk_queue(void);

static dir* _new_root;
dir* new_root(void) {
  return _new_root;
}

void setup(void) {
  printk("setup");
  ksetup_sched();
  printk("after sched");
  setup_ko_registry();
  printk("done with registry");
  setup_err_ko();

  _new_root = mk_dir();
  printk("root %k", _new_root);
  KO(new_root())->rc = 99;

  SAFE_ADD(new_root(), mk_fs(), "fs");

  dir* proc = mk_dir();
  LINK(new_root(), proc, "proc");
  //SAFE_ADD(proc, mk_ctor(proc_me, KO_DIR), "me");
  printk("proc %k", proc);
  assert(IS_DIR(proc));
  kput(proc);

  //SAFE_ADD(new_root(), mk_sched(), "sch");

}

void enable_irq(int clobber);

void kmain(void) {
  ksetup_memory();
  printk("Starting main");
  printk("begin cspr %x", __get_CPSR());
//enable_interrupt();
  enable_irq(0);
  printk("end cspr %x", __get_CPSR());
  printk("begin mask %x", get_irq_mask());
  enable_rtc_irq();
  set_timer(100);
  enable_cons_irq();
  printk("end mask %x", get_irq_mask());
  
  for (;;) {}

  //enable_mmu();


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
  printk("here");

  // kflush_file(root());
  restore_pcb(&p->pcb);
}

