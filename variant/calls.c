#include "variant.h"

static variant *registry = 0;

variant dispatch(variant name) {
  return get(*registry, name);
}

static void reg(variant name, variant func) {
  set(*registry, name, func);
}

#define ARGS(n) do { if (len(args) < n) { \
  printk("Func needs exactly %d args, got %d", n, len(args)); return Null(); } } while (0)

#define A(n) idx(args, n)

#define VFUNC(func) variant func(variant args)

VFUNC(f_get) {
  ARGS(2);
  return get(A(0), A(1));
}

VFUNC(f_set) {
  ARGS(3);
  set(A(0), A(1), A(2));
  return Null();
}

VFUNC(f_id) {
  ARGS(1);
  printk("lol");
  return inc(A(0));
}

VFUNC(f_idx) {
  ARGS(2);
  if (!IS_I(A(1))) { return Null(); }
  return inc(idx(A(0), A(1).i));
}

VFUNC(f_len) {
  ARGS(1);
  printk("len %v", A(0));
  return Int(len(A(0)));
}

VFUNC(f_slice) {
  ARGS(3);
  if (!IS_I(A(1))) { return Null(); }
  if (!IS_I(A(2))) { return Null(); }
  return slice(A(0), A(1).i, A(2).i);
}

VFUNC(f_eq) {
  ARGS(2);
  return Int(eq(A(0), A(1)));
}

VFUNC(f_list) {
  return inc(args); // heh
}

variant call(variant f, variant args);

VFUNC(f_apply) {
  printk("Apply");
  if (len(args) < 2) {
    return Null();
  }
  variant v = List(len(args));
  printk("A1 %v", A(1));
  foreach(x, A(1)) {
    printk("calling %v on %v", A(0), x);
    V(l) = List(1);
    V(r) = call(A(0), x);
    push(l, r);
    printk("%v", l);
    push(v, l);
  }
  return v;
}

#define REG(func, name) do { V(f) = Func(func, name); reg(CStr(name), f); } while(0)

void init_calls() {
  printk("INIT");
  registry = malloc(sizeof(variant));
  *registry = Map(4);

  printk("id %p", f_id);

  REG(f_get, "get");
  REG(f_set, "set!");
  REG(f_idx, "idx");
  REG(f_len, "len");
  REG(f_slice, "slice");
  REG(f_eq, "eq");
  REG(f_id, "id");
  REG(f_list, "list");
  REG(f_apply, "apply");
}

