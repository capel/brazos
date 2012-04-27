#include "task.h"
#include "mach.h"

void set_ct(task* t);

static fountain* task_queue;
static sinkhole* task_queue_start;
void setup_sched() {
  dir* d = mk_queue();
  task_queue_start = kget(LOOKUP(d, "push"));
  task_queue = kget(LOOKUP(d, "pop"));
}

static void cleanup(ko* o) {
  task* t = TASK(o);
  kput(t->rids);
  kput(t->d);
  kfree_pages(t->stack);
  // somehow actually clean up... kfree(t->vm);
}

static msg* view(ko* o) {
  return VIEW(TASK(o)->d);
}

static ko* lookup(dir* o, const char* name) {
  return LOOKUP(TASK(o)->d, name);
}

static ko* link(dir* d, ko* o, const char* name) {
  return LINK(TASK(o)->d, o, name);
}

static ko* unlink(dir* d, const char* name) {
  return UNLINK(TASK(d)->d, name);
}

static dir_vtable vt = {
  .lookup = lookup,
  .link = insert,
  .unlink = delete,
};

void idle_task(void);

task* mk_task(void* start_addr) {
  task* t = mk_ko(sizeof(task), cleanup, view, KO_DIR);
  SET_FLAG(t, KO_TASK);
  DIR(t)->v = &vt;

  t->d = mk_dir();
  t->rids = mk_idir();
  t->runnable = 1;
  t->stack = kget_pages(USER_STACK_SIZE) + USER_STACK_SIZE * PAGE_SIZE;
  t->vm = get_vm_base();

  LINK(p->d, p->rids, "rids");
  LINK(p->d, root(), "cwd"); 

  t->pcb.spsr = __get_CPSR();
  t->pcb.spsr |= 0x80; // disable interrupts
  t->pcb.pc = start_addr;
  t->pcb.lr = sched;
  t->pcb.sp = p->stack;

  return t;
}

task* mk_user_task(void* start_addr) {
  task* t = mk_task(start_addr);

  // clear flags and set to user mode
  t->pcb.spsr &= (~0x1f);
  t->pcb.spsr |= 0x10;
  t->pcb.spsr &= (~0x80); // enable interrupts in user procs

  t->vm = kmalloc(sizeof(vm_data));
  memset((void*)p->vm, 0, sizeof(vm_data));
  
  return t;
}

void kcopy_pcb(task *t, PCB *pcb) {
    if (t->pcb == pcb) {
        printk("current and src are the same [%p]?", pcb);
    }
    memcpy((char*)&t->pcb, (char*) pcb, sizeof(PCB));
}

static task* current;
task* ct() {
  return current;
}

void restore_pcb(struct regs* regs);
void save_state();
void raw_jump(void* location) __attribute__(noreturn);

void ksave_state(PCB *pcb) {
  kcopy_pcb(ct(), stacked_pcb);
  PCB* pcb = &ct()->pcb;
  raw_jump(ct()->pcb.lr);
}


void set_ct(task*t) {
  current = t;
  set_vm_base(t->vm);
  restore_pcb(&t->regs);
}

void enqueue_task(task* t) {
  SINK(task_queue, t);
}

void sched() {
  save_state(ct());
  enqueue_task(ct());
  _sched();
}


void _sched() {
  // let interrupts fire
  enable_interrupt();
  disable_interrupt();

  task* t = q_pop(task_queue);
  set_ct(t);
}

void task_wait(task* t) {
  t->runnable = 0;
}

void task_wake(task* t) {
  t->runnable = 1;
}

int proc_add_ko(task* p, ko* o) {
  if (!o) return 0;
  if (ID(o) < 0) return ID(o);
  idir_insert(p->rids, ID(o), o);
  return ID(o);
}

ko* proc_rid(task* p, int rid) {
  if (rid < 0) return get_ko(rid);
  return idir_lookup(p->rids, rid);
}

