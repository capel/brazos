#include "sched.h"
#include "stdlib.h"
#include "include/common.h"
#include "include/sched.h"
#include "include/ent_gen.h"
#include "include/proc.h"

proc * current;
sched* scheduler;

typedef struct {
  size_t min_stride;
  size_t cur_pid;
} sched_data;

#define DATA(sch) ((sched_data*)(sch->d1))
#define PROCS(sch) ((vector*)(sch->d2))

proc * new_proc(sched * sch, actor *a, vector* v, size_t level)
{
  proc* p = kcreate_proc(DATA(sch)->cur_pid, a);
  LINK_R(sch, a, p, kitoa(DATA(sch)->cur_pid));
  DATA(sch)->cur_pid++;
  return kget(p);
}

void kcopy_pcb(PCB *pcb) {
    if (&PROC_PCB(current) == pcb) {
        printk("current and src are the same [%p]?", pcb);
    }
    memcpy((char*)&PROC_PCB(current), (char*) pcb, sizeof(PCB));
}

bool better(void *a, void *b) {
  return PROC_STRIDE((ent*)a) < PROC_STRIDE((ent*)b);
}

proc* ksched()
{
  if (PROCS(scheduler)->size == 0) {
      panic("No procs. Goodbye world!");
  }
  proc* p = vector_best(scheduler->d2, better);
  DATA(scheduler)->min_stride = PROC_STRIDE(p);
  PROC_STRIDE(p) += 10;
  current = p;
  return p;
}

proc * cp(void) {
    return current;
}

static ent_lookup internal_funcs[] = {
  {DEFAULT_LOOKUP_FUNC, managed_lookup_func_not_found},
  {"new!", new_proc}
};

MAKE_LOOKUP(internal_funcs);
MAKE_MAP(internal_funcs);
MAKE_UNMAP(internal_funcs);

static void cleanup(ent* e) {}

static ent_funcs sched_funcs = {
  .lookup = NAME_LOOKUP(internal_funcs),
  .link = disable_link,
  .unlink = disable_unlink,
  .map = NAME_MAP(internal_funcs),
  .unmap = NAME_UNMAP(internal_funcs),
  .cleanup = cleanup,
};

sched* kcreate_sched()
{
  sched *sch = entalloc(&sched_funcs);
  scheduler = sch;
  sch->d1 = kmalloc(sizeof(sched_data));
  DATA(sch)->min_stride = 0;
  DATA(sch)->cur_pid = 1;
  return sch;
}

