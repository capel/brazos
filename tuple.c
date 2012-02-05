// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "vv.h"

static variant null_tuple() {
  variant v = Null();
  v.type = V_T;
  v.len = 0;
  v.t = 0;
  return v;
}

static variant _tuple(size_t len) {
  if (len == 0) return null_tuple();

  variant t = Variant(V_T);
  t.len = len;
  t.t = malloc(sizeof(variant) * len);
  return t;
}

bool tuple_eq(variant a, variant b) {
  if (len(a) != len(b)) return false;

  for(size_t i = 0; i < len(a); i++) {
    if (!eq(idx(a, i), idx(b, i))) {
      return false;
    }
  }
  return true;
}

variant Tuple(size_t len, ...) {
  if (len == 0) return null_tuple();

  variant t = _tuple(len);

  va_list va;
  va_start(va, len);
  for(size_t i = 0; i < len; i++) {
    t.t[i] = va_arg(va, variant);
    inc(t.t[i]);
  }

  va_end(va);
  return t;
}

void tuple_cleanup(variant t) {
  foreach(x, t) {
    dec(x);
  }
  free(t.t);
}

variant tuple_serialize(variant t) {
  if (len(t) == 0) return CStr("[]");
  VV(v) = Vv(len(t) + 1);
  push(v, CStr("tuple"));
  foreach(x, t) {
    push(v, x);
  }

  variant s = join(v, ',', '[', ']', true);
  return s;
}


variant Tvv(vv * v) {
  if (vvlen(v) == 0) {
    return _tuple(0);
  }
  variant t = _tuple(vvlen(v));
  for (size_t i = 0; i < vvlen(v); i++) {
    t.t[i] = vvidx(v,i); 
    inc(t.t[i]);
  }

  return t;
}

