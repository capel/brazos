#include "vv.h"
#include "malloc.h"

typedef struct vv {
  size_t size;
  size_t alloced;
  variant* data;
} vv;

vv * Vv(size_t init_size) {
  vv * v = malloc(sizeof(vv));
  v->size = 0;
  v->alloced = init_size;
  v->data = calloc(sizeof(variant), init_size);
  return v;
}

void push(vv* v, variant d) {
  if (v->size >= v->alloced) {
    variant* tmp = calloc(sizeof(variant), v->alloced * 2);
    memcpy((void*)tmp, (void*)v->data, v->alloced * sizeof(variant));
    free(v->data);
    v->data = tmp;
    v->alloced *= 2;
  }

  v->data[v->size++] = d;
  inc(d);
}

variant vvidx(vv* v, size_t idx) {
  return v->data[idx];
}

size_t vvlen(vv* v) {
  return v->size;
}

void vvdec(vv* v) {
  for(size_t i = 0; i < v->size; i++) {
    dec(vvidx(v, i));
  }
  free(v->data);
  free(v);
}

#define PRINT(x) bufpos = strrcpy(buf, bufpos, size, (x))
#define PRINTC(c) buf[bufpos++] = (c);
variant join(vv* v, char sep, char start, char end, bool serialize_strings) {
  vv * t = Vv(vvlen(v));
  size_t req = 0;
  vvforeach(q, v) {
    variant s;
    if (!serialize_strings && IS_S(q)) {
      s = q;
    } else {
      s = serialize(q);
    }
    req += len(s);
    push(t, s);
  }

  size_t size = req + 2 + vvlen(v);
  char* buf = malloc(size);
  size_t bufpos = 0;
  
  PRINTC(start);

  for(size_t i = 0; i < vvlen(t); i++) {
    PRINT(vvidx(t, i).s);
    if (i != vvlen(t) -1) {
      PRINTC(sep);
    }
  }


  PRINTC(end);
  PRINTC('\0');

  vvdec(t);
  variant s = RStr(buf);

  return s;
}

#undef PRINT
#undef PRINTC
