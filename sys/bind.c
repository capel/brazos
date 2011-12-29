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

const char* bound_view(ko* o) {
  return "BOUND!";
}

ko* bind(bound_func func, int type, void* data) {
  bound * b = (bound*) mk_ko(sizeof(bound), bound_cleanup, bound_view, type);

  SET_FLAG(b, KO_BOUND);

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

static const char* sinkhole_view(ko* sh) {
  return "SINKHOLE";
}

sinkhole* mk_sinkhole(sink_func func, void* data) {
  sinkhole * b = (sinkhole*) mk_ko(sizeof(sinkhole), sinkhole_cleanup, 
      sinkhole_view, KO_SINKHOLE);
  b->sink = func;
  b->data = data;
  return b;
}
