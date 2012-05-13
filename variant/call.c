#include "variant.h"
#include "parse_macro.h"

variant dispatch(variant fname);

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
  return serialize(*f.fname);
}

variant call(variant name, variant args) {
  if (!IS_S(name)) { 
    printk("not s... %v", name);
    return Null(); // bad news bears
  }
  variant f = dispatch(name);
  printk("%v %c %p", f, f.type, f.f);
  if (!IS_F(f)) {
    printk("not f... %v", f);
    return Null();
  }
  printk("About to eval");
  variant e = eval(args);
  printk("evaled %v", e);
  return f.f(e);
}

variant call_eval(variant t) {
  if (len(t) == 0) return t;

  variant s = sslice(t, 1);
  printk("%v %v", idx(t,0), s);
  return call(idx(t,0), s);
}

#include "parse_macro.h"

PARSE(call_parse) {
  CONSUME('[');
  NOM_SPACE();
  if (s[*pos] == ']') {
    CONSUME(']');
    return List(0);
  }

  V(v) = List(5);
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

  inc(v);
  v.callable = true;

  return v;
}

