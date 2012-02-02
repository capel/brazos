#ifndef VV_H
#define VV_H

#include "variant.h"

typedef struct vv vv;

vv* Vv(size_t init_size);
void push(vv*, variant);
variant vvidx(vv*, size_t idx);
size_t vvlen(vv*);

void vvdec(vv*);
static inline void pvvdec(vv** v) {
  vvdec(*v);
}

variant join(vv*, char sep, char start, char end, bool serialize_strings);

#define VV(x) vv* x __attribute__((cleanup(pvvdec))) 

#define vvforeach(x, v) \
  size_t __j_##x = 0; \
  for(variant x = vvidx(v, 0); __j_##x < vvlen(v); x = vvidx(v, ++__j_##x))

#endif
