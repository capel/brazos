#include "include/proc.h"
#include "include/mem.h"

static void bad_main() {
  panic("Bad main called!!!");
}

proc * kcreate_proc(size_t pid, actor* owner) {
    proc *p = kmalloc(sizeof(*p));
    assert(p);
    printk("%p", p);
    p->pid = pid;
    p->cwd = root();
    p->actor = owner;

    unsigned spsr = __get_CPSR();
    p->pcb.spsr = spsr & (~0x1f);
    p->pcb.spsr |= 0x10;
    p->pcb.pc = bad_main;
    p->pcb.lr = default_exit;
    p->stack = kget_pages(USER_STACK_SIZE) + USER_STACK_SIZE * PAGE_SIZE;
    p->pcb.sp = p->stack;
}

static void exec(proc* p, actor* a, program* main) {
    p->pcb.sp = p->stack;
    p->pcb.lr = default_exit;
    size_t ignore;
    MAP_R(main, a, P_R, &size, &p->pcb.pc);
}

static void cleanup(proc *p) {}

static ent_funcs proc_funcs = {
  .lookup = disable_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = cleanup,
};

