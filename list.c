#include "variant.h"
#include "malloc.h"

variant List(size_t init_size) {
  variant v = Variant(V_L);
  v.l = malloc(sizeof(ldata));
  v.l->len = 0;
  v.l->alloced = init_size;
  v.l->t = calloc(sizeof(variant), init_size);
  v.callable = 0;
  return v;
}

void push(variant v, variant d) {
  if (v.l->len >= v.l->alloced) {
    variant* tmp = calloc(sizeof(variant), v.l->alloced * 2);
    memcpy((void*)tmp, (void*)v.l->t, v.l->alloced * sizeof(variant));
    free(v.l->t);
    v.l->t = tmp;
    v.l->alloced *= 2;
  }

  v.l->t[v.l->len++] = d;
  inc(d);
}

void list_cleanup(variant v) {
  foreach(x, v) {
    dec(x);
  }
  free(v.l);
}

bool list_eq(variant a, variant b) {
  if (len(a) != len(b)) return false;
  fori(i, len(a)) {
      if (!eq(idx(a, i), idx(b, i))) return false;
  }
  return true;
}

#define PRINT(x) bufpos = strrcpy(buf, bufpos, size, (x))
#define PRINTC(c) buf[bufpos++] = (c);
variant join(variant v, char sep, char start, char end, bool serialize_strings) {
  V(t) = List(len(v));
  size_t req = 0;
  foreach(q, v) {
    variant s;
    if (!serialize_strings && IS_S(q)) {
      s = q;
    } else {
      s = serialize(q);
    }
    req += len(s);
    push(t, s);
  }

  size_t size = req + 2 + len(v);
  char* buf = malloc(size);
  size_t bufpos = 0;
  
  PRINTC(start);

  fori(i, len(t)) {
    PRINT(idx(t, i).s);
    if (i != len(t) -1) {
      PRINTC(sep);
    }
  }


  PRINTC(end);
  PRINTC('\0');

  return RStr(buf);
}

variant call_eval(variant v);
variant list_eval(variant l) {
  if (l.callable) return call_eval(l);

  variant v = List(len(l));
  foreach(x, l) {
    push(v, eval(x));
  }
  return v;
}

variant list_serialize(variant t) {
  if (len(t) == 0) return CStr("[]");
  
  if (t.callable) {
    return join(t, ',', '[', ']', true);
  }

  V(v) = List(len(t) + 1);
  push(v, CStr("list"));
  foreach(x, t) {
    push(v, x);
  }

  return join(v, ',', '[', ']', true);
}

#undef PRINT
#undef PRINTC
