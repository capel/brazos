#include "include/proc.h"
#include "include/common.h"
#include "include/kfs.h"
#include "include/ent_gen.h"
#include "include/mach.h"

ent* kcreate_rid_manager(void);

static void bad_main() {
  panic("Bad main called!!!");
}
static void default_exit() {
  panic("default exit.");
}

typedef ent program;

static void exec(proc* p, program* main) {
    PROC_PCB(p).sp = PROC_DATA(p)->stack;
    PROC_PCB(p).lr = default_exit;
    size_t ignore;
    MAP(main, P_R, &ignore, &PROC_PCB(p).pc);
}

static void cleanup(proc *p) { printk("TODO"); }

static ent_funcs proc_funcs = {
  .lookup = simple_managed_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = cleanup
};

proc * kcreate_proc(size_t pid) {
    proc *p = entalloc(&proc_funcs);
    printk("new proc %p", p);
    p->d1 = kmalloc(sizeof(proc_data));
    PROC_DATA(p)->pid = pid;
    PROC_DATA(p)->cwd = root();

    unsigned spsr = __get_CPSR();
    PROC_PCB(p).spsr = spsr & (~0x1f);
    printk("%x", PROC_PCB(p).spsr);
    PROC_PCB(p).spsr |= 0x10;
    printk("%x", PROC_PCB(p).spsr);
    PROC_PCB(p).pc = bad_main;
    PROC_PCB(p).lr = default_exit;
    pages* pp = FS("/mm/new!/%d", USER_STACK_SIZE);
    size_t size;
    void* mem;
    MAP(pp, P_W, &size, &mem);
    PROC_DATA(p)->stack = mem + size;
    PROC_PCB(p).sp = PROC_DATA(p)->stack;

    ent* ridm = kcreate_rid_manager();
    p->f->link = simple_managed_link;
    err_t err = LINK_R(p, ridm, "rid");
    if (!OK(err)) printk("err: %d", err);
    p->f->link = disable_link;

    return p;
}
