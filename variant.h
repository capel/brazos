#ifndef VARIANT_H
#define VARIANT_H

#include "types.h"
#include "stdlib.h"

typedef struct vv vv;

typedef enum {
  V_S = 'S',
  V_M = 'M',
  V_T = 'T',
  V_H = 'H',
  V_I = 'I',
  V_C = 'C',
  V_N = '\0',
} vtype;

typedef enum {
  H_H,
  H_F
} htype;

#define IS_S(x) ((x).type == V_S)
#define IS_M(x) ((x).type == V_M)
#define IS_T(x) ((x).type == V_T)
#define IS_K(x) ((x).type == V_K)
#define IS_H(x) ((x).type == V_H)
#define IS_I(x) ((x).type == V_I)
#define IS_N(x) ((x).type == V_N)
#define IS_C(x) ((x).type == V_C)

#define IS_HF(x) ((x).subtype == H_F)
#define IS_HH(x) ((x).subtype == H_H)

typedef struct variant (*vfunc)(struct variant argst);

typedef struct variant {
  vtype type;
  size_t* rc;
  union {
    int i;
    const char * s;
    struct variant * t;
    void* h;
    vfunc f;
  };
  union {
    size_t len;
    htype subtype;
    size_t bucket_power;
  };
} variant;

#define V(x) variant x __attribute__((cleanup(pdec))) 

inline static void inc(variant v) {
  if (!v.rc) return;
  *v.rc += 1;
}

void _dec(variant v);
static inline void pdec(variant * v) {
  if (!v->rc) return;
  *v->rc -= 1;
  if (*(v->rc) == 0) {
    _dec(*v);
  }
}

inline static void dec(variant v) {
  if (!v.rc) return;
  *(v.rc) -= 1;
  if (*(v.rc) == 0) {
    _dec(v);
  }
}

size_t len(variant v);
variant idx(variant v, size_t pos);
variant slice(variant v, size_t start, size_t end);
variant sslice(variant v, size_t start);
variant eslice(variant v, size_t end);

void set(variant m, variant key, variant v);
variant get(variant m, variant key);

bool eq(variant a, variant b);
unsigned hash(variant v);

variant parse(const char* s);
variant serialize(const variant v);


variant Tuple(size_t len, ...);
#define T2(x,y) Tuple(2, (x), (y))
#define T3(x,y,z) Tuple(3, (x), (y), (z))
variant Tvv(vv* v);

variant Variant(vtype type); 

variant Handle(void* h, htype subtype);

variant Map(size_t power2_num_buckets);
variant Str(const char* s);
variant RStr(const char* s);
variant CStr(const char* s);
variant Int(int i);

#define Null() Variant(V_N)

#define VFUNC(func) variant func(variant args)

#define foreach(x, v) \
  size_t __i_##x = 0; \
  for(variant x = idx(v, 0); __i_##x < len(v); x = idx(v, ++__i_##x))

#endif

