#include "include/common.h"
#include "include/ent_gen.h"

typedef ent sched;

typedef struct {
  size_t min_stride;
  size_t cur_pid;
  proc* current;
} sched_data;

#define DATA(sch) ((sched_data*)(sch->d1))
#define PROCS(sch) ((vector*)(sch->d2))

static proc * new_proc(sched * sch, const vector* v, size_t level, bool *done)
{
  printk("v %p", v);
  proc* p = mkproc(DATA(sch)->cur_pid);
  printk("new proc sched %p", p);
  
  sch->f->link = simple_managed_link;
  err_t err = LINK_R(sch, p, "%d", DATA(sch)->cur_pid);
  printk("err: %d", err);
  sch->f->link = disable_link;
  DATA(sch)->cur_pid++;
  *done = true;
  return kget(p);
}

void kcopy_pcb(PCB *pcb) {
    proc* current = FS("/proc/me");
    if (&PROC_PCB(current) == pcb) {
        printk("current and src are the same [%p]?", pcb);
    }
    memcpy((char*)&PROC_PCB(current), (char*) pcb, sizeof(PCB));
}

bool better(void *a, void *b) {
  assert(a);
  assert(b);
  return PROC_STRIDE(GET_ENT(a)) < PROC_STRIDE(GET_ENT(b));
}

static proc* do_sched(sched * sch, const vector* v, size_t level, bool *done)
{
  if (PROCS(sch)->size == 0) {
      panic("No procs. Goodbye world!");
  }
  proc* p = GET_ENT(vector_best(sch->d2, better));
  DATA(sch)->min_stride = PROC_STRIDE(p);
  PROC_STRIDE(p) += 10;
  DATA(sch)->current = p;
  printk("Sched! proc %p", p);
  *done = true;
  return kget(p);
}

static proc* me(sched *sch, const vector* v, 
    size_t level, bool *done) {
  printk("current:", DATA(sch)->current);
  return kget(DATA(sch)->current);
}


static ent_lookup internal_funcs[] = {
  {DEFAULT_LOOKUP_FUNC, managed_lookup_func_not_found},
  {"new!", new_proc},
  {"sched!", do_sched},
  {"me", me},
};

MAKE_LOOKUP(internal_funcs);
MAKE_MAP(internal_funcs);
MAKE_UNMAP(internal_funcs);

static void cleanup(ent* e) { panic("Sched was cleaned up!"); }

static ent_funcs sched_funcs = {
  .lookup = NAME_LOOKUP(internal_funcs),
  .link = disable_link,
  .unlink = disable_unlink,
  .map = NAME_MAP(internal_funcs),
  .unmap = NAME_UNMAP(internal_funcs),
  .cleanup = cleanup,
};

sched* mksched()
{
  sched *sch = entalloc(&sched_funcs);
  sch->d1 = kmalloc(sizeof(sched_data));
  DATA(sch)->min_stride = 0;
  DATA(sch)->cur_pid = 1;
  DATA(sch)->current = NULL;
  simple_managed_create(sch);
  return sch;
}

