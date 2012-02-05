#include "variant.h"
#include "parse_macro.h"
#include "vv.h"

static variant *registry = 0;

static variant dispatch(variant name) {
  return get(*registry, name);
}

static void reg(variant name, variant func) {
  set(*registry, name, func);
}

#define ARGS(n) do { if (len(args) != n) { \
  printk("Func needs exactly %n arg, got %d", n, len(args)); return Null(); } } while (0)

#define A(n) idx(args, n)

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
  return A(0);
}

VFUNC(f_idx) {
  ARGS(2);
  if (!IS_I(A(1))) { return Null(); }
  return idx(A(0), A(1).i);
}

VFUNC(f_len) {
  ARGS(1);
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

VFUNC(f_tuple) {
  return args; // heh
}

#define REG(func, name) do { V(f) = Func(func, name); reg(CStr(name), f); } while(0)

void init_calls() {
  printk("INIT");
  registry = malloc(sizeof(variant));
  *registry = Map(4);

  REG(f_get, "get");
  REG(f_set, "set");
  REG(f_idx, "idx");
  REG(f_len, "len");
  REG(f_slice, "slice");
  REG(f_eq, "eq");
  REG(f_id, "id");
  REG(f_tuple, "tuple");
  printk("%v", *registry);
}

variant Func(vfunc f, const char* name) {
  variant v = Variant(V_F);
  v.f = f;

  v.fname = malloc(sizeof(variant));
  *v.fname = Str(name);
  return v;
}

void func_cleanup(variant f) {
  dec(*f.fname);
}

variant func_serialize(variant f) {
  return *f.fname;
}

variant call_eval(variant t) {
  if (len(t) == 0) return t;

  variant s = idx(t, 0);
  if (!IS_S(s)) { 
    printk("not s... %v", s);
    return Null(); // bad news bears
  }
  variant f = dispatch(s);
  if (!IS_F(f)) {
    printk("not f... %v", f);
    return Null();
  }
  V(args) = sslice(t, 1);
  printk("%v %v", t, args);

  return f.f(args);
}

#include "parse_macro.h"

PARSE(call_parse) {
  CONSUME('[');
  NOM_SPACE();
  if (s[*pos] == ']') {
    CONSUME(']');
    return Tuple(0);
  }

  VV(v) = Vv(5); // a decent guess
  V(tmp);
  do {
    tmp = _parse(s, pos, die);
    if (*die) DIE();

    push(v, tmp);
    NOM_SPACE();
    if (s[*pos] != ']') {
      CONSUME(',');
    }
  } while(s[*pos] != ']');

  CONSUME(']');

  variant t = Tvv(v);
  t.type = V_C; // they are calls if they are literals

  return t;
}

variant call_serialize(variant t) {
  VV(v) = Vv(len(t));
  foreach(x, t) {
    push(v, x);
  }

  variant s = join(v, ',', '[', ']', true);
  return s;
}


variant Handle(void* h, htype subtype) {
  variant v = Variant(V_H);
  v.subtype = subtype;
  v.h = h;
  return v;
}
