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

#define ARGS(n) do { if (len(args) != 2) { \
  printk("Need more args %d", len(args)); return Null(); } } while (0)

VFUNC(f_get) {
  ARGS(2);
  return get(idx(args,0), idx(args,1));
}

void init_calls() {
  registry = malloc(sizeof(variant));
  *registry = Map(4);

  reg(CStr("get"), Handle(f_get, H_F));
}

static PARSE(call_inner_parse) {
  CONSUME('[');

  VV(v) = Vv(5); // a decent guess
  do {
    V(tmp) = _parse(s, pos, die);
    HOPE(*die);

    push(v, tmp);
  } while(*s && s[*pos] != ']');
  CONSUME(']');

  return Tvv(v);
}

PARSE(call_parse) {
  variant c = call_inner_parse(s, pos, die);
  if (*die) { dec(c); return Null(); }

  printk("done");
  printk("%v", c);

  HOPE(IS_T(c));
  HOPE(len(c) >= 1);
  HOPE(IS_S(idx(c, 0)));

  c.type = V_C;
  return c;
}

variant call_serialize(variant t) {
  assert(IS_C(t));

  vv * v = Vv(len(t));
  foreach(x, t) {
    push(v, x);
  }

  variant s = join(v, ' ', '[', ']', true);
  vvdec(v);
  return s;
}

variant eval(variant call) {
  assert(IS_C(call));
  
  variant h = dispatch(idx(call, 0));
  if (IS_N(h)) return Null();

  assert(IS_H(h));
  assert(IS_HF(h));
  
  return h.f(sslice(call, 1));
}

variant Handle(void* h, htype subtype) {
  variant v = Variant(V_H);
  v.subtype = subtype;
  v.h = h;
  return v;
}
