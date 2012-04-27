#ifndef VARIANT_H
#define VARIANT_H

#include "types.h"
#include "stdlib.h"

typedef enum {
  V_S = 'S',
  V_M = 'M',
  V_L = 'L',
  V_I = 'I',
  V_F = 'F',
  V_N = '\0',
} vtype;

#define IS_S(x) ((x).type == V_S)
#define IS_M(x) ((x).type == V_M)
#define IS_T(x) ((x).type == V_T)
#define IS_C(x) (IS_L(x) && (x).call)
#define IS_I(x) ((x).type == V_I)
#define IS_N(x) ((x).type == V_N)
#define IS_F(x) ((x).type == V_F)

#define IS_HF(x) ((x).subtype == H_F)
#define IS_HH(x) ((x).subtype == H_H)

typedef struct variant (*vfunc)(struct variant argst);

typedef struct {
  size_t len;
  size_t alloced;
  struct variant* t;
} ldata;

typedef struct variant {
  vtype type;
  size_t* rc;
  union {
    int i;
    const char * s;
    struct variant * t;
    vfunc f;
    bool callable;
  };
  union {
    ldata* l;
    size_t len;
    size_t bucket_power;
    struct variant* fname;
  };
} variant;

#define V(x) variant x __attribute__((cleanup(pdec))) 

inline static variant inc(variant v) {
  if (!v.rc) return v;
  *v.rc += 1;
  return v;
}

void _dec(variant v);
void pdec(variant * v);
//void dec(variant v);

#define dec(v) do { if (!(v).rc) break; if (--*((v).rc) == 0) _dec(v); } while(0)

size_t len(variant v);
variant idx(variant v, size_t pos);
variant slice(variant v, size_t start, size_t end);
variant sslice(variant v, size_t start);
variant eslice(variant v, size_t end);

void push(variant list, variant a);
variant join(variant v, char sep, char start, char end, bool serialize_strings);

void set(variant m, variant key, variant v);
variant get(variant m, variant key);

bool eq(variant a, variant b);
unsigned hash(variant v);
variant eval(variant v);

variant parse(const char* s);
variant serialize(const variant v);

variant List(size_t len);

variant Variant(vtype type); 


variant Map(size_t power2_num_buckets);
variant Str(const char* s);
variant RStr(const char* s);
variant CStr(const char* s);
variant Int(int i);
variant Func(vfunc f, const char *name);

#define Null() Variant(V_N)


#define fori(x, n) for(size_t i = 0; i < n; i++)

#define foreach(x, v) \
  size_t __i_##x = 0; \
  for(variant x = idx(v, 0); __i_##x < len(v); x = idx(v, ++__i_##x))

#endif

