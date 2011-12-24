#include "ko.h"
#include "../stdlib.h"
#include "../mem.h"

typedef struct {
  ko o;
  bound_func func;
  void* data;
} bound;

static void bound_cleanup(ko* o) {
  // ???
}

ko* bind(bound_func func, void* data) {
  bound * b = kmalloc(sizeof(bound));
  KO(b)->cleanup = bound_cleanup;
  KO(b)->type = KO_BOUND;
  KO(b)->rc = 1;

  b->func = func;
  b->data = data;
  return KO(b);
}

ko* release(ko* o) {
  assert(IS_BOUND(o));
  bound * b = (bound*) o;
  return b->func(b->data);
}

static void sinkhole_cleanup(ko* sh) {
  printk("sinkhole cleaned up %k", sh);
}

sinkhole* mk_sinkhole(sink_func func, void* data) {
  sinkhole * b = kmalloc(sizeof(sinkhole));
  KO(b)->cleanup = sinkhole_cleanup;
  KO(b)->type = KO_SINKHOLE;
  KO(b)->rc = 1;
  b->sink = func;
  b->data = data;
  return b;
}
