#include "variant.h"
#include "vv.h"
#include "stdlib.h"
#include "stdio.h"

#define KEY(x) idx(x, 0)
#define VALUE(x) idx(x, 1)
#define NEXT(x) idx(x, 2)

#define MASK(v) ((size_t)((1 << (v).bucket_power) - 1))
#define SIZE(v) ((size_t)(1 << (v).bucket_power))

void set(variant m, variant key, variant value) {
  assert(IS_M(m));
  unsigned b = hash(key) & MASK(m);

  inc(key);
  inc(value);

  m.t[b] = T3(key, value, m.t[b]);
}

static variant delve(variant t, variant key) {
  if (IS_N(t)) return Null();

  assert(IS_T(t));
  if (eq(KEY(t), key)) {
    return VALUE(t);
  } else {
    return delve(NEXT(t), key);
  }
}

variant get(variant m, variant key) {
  assert(IS_M(m));

  unsigned b = hash(key) & MASK(m);
  return delve(m.t[b], key);
}

static void pushv(vv* v, variant b) {
  if (IS_N(b)) return;

  V(key) = serialize(KEY(b));
  V(val) = serialize(VALUE(b));

  size_t needed = len(key) + 1 + len(val) + 1;
  char* s = malloc(needed);
  snprintf(s, needed, "%s:%s", key.s, val.s);
  V(r) = RStr(s);

  push(v, r);
  pushv(v, NEXT(b));
}

static vv* toList(variant m) {
  vv * v = Vv(SIZE(m) * 3);
  for(size_t i = 0; i < SIZE(m); i++) {
    pushv(v, m.t[i]);
  }
  return v;
}

void map_cleanup(variant m) {
  for (size_t i = 0; i < SIZE(m); i++) {
    dec(m.t[i]);
  }
  free(m.t);
}

variant map_serialize(variant m) {
  VV(v) = toList(m);
  return join(v, ',', '{', '}', false);
}

variant Map(size_t power2_num_buckets) {
  variant m = Variant(V_M);
  m.bucket_power = power2_num_buckets;
  m.t = calloc(sizeof(variant), SIZE(m));
  return m;
}

#include "parse_macro.h"

PARSE(map_parse) {
  CONSUME('{');
  NOM_SPACE();
  if (s[*pos] == '}') {
    CONSUME('}');
    return Map(3);
  }

  V(m) = Map(3);
  do {
    V(k) = _parse(s, pos, die);
    HOPE(!*die);

    NOM_SPACE();
    CONSUME(':');

    V(v)  = _parse(s, pos, die);
    HOPE(!*die);

    set(m, k, v);

    NOM_SPACE();
    if (s[*pos] != '}') {
      CONSUME(',');
    }
  } while (*s && s[*pos] != '}');
  CONSUME('}');

  inc(m); // cause its auto dec
  return m;
}

