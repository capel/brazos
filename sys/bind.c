#include "ko.h"
#include "../stdlib.h"
#include "../mem.h"

typedef struct {
  ko o;
  window_func func;
  void* data;
} window;

static void window_cleanup(ko* o) {}

static msg* window_view(ko* o) {
  assert(IS_WINDOW(o));
  window * w = (window*) o;
  return w->func(w->data);
}

ko* mk_window(window_func func, void* data) {
  window * w = (window*) mk_ko(sizeof(window), window_cleanup, window_view, KO_MESSAGE);

  SET_FLAG(w, KO_WINDOW);

  w->func = func;
  w->data = data;
  return KO(w);
}

static void sinkhole_cleanup(ko* sh) {
  printk("sinkhole cleaned up %k", sh);
}

static msg* sinkhole_view(ko* sh) {
  return mk_msg("SINKHOLE");
}

sinkhole* mk_sinkhole(sink_func func, void* data) {
  sinkhole * b = (sinkhole*) mk_ko(sizeof(sinkhole), sinkhole_cleanup, 
      sinkhole_view, KO_SINKHOLE);
  b->sink = func;
  b->data = data;
  return b;
}

typedef struct ctor {
  ko o;
  ctor_func construct;
} ctor;

ko* construct(ko * o) {
  assert(IS_CTOR(o));
  ctor* c = (ctor*)o;
  return c->construct();
}

static void ctor_cleanup(ko* o) {}
static msg* ctor_view(ko* o) { assert(false); }

ko* mk_ctor(ctor_func func, int type) {
  ctor * b = (ctor*) mk_ko(sizeof(ctor), ctor_cleanup, 
      ctor_view, type);
  b->construct = func;
  SET_FLAG(b, KO_CTOR);
  return KO(b);
}

static void f_cleanup(ko* o) {}
static msg* f_view(ko* o) { return mk_msg("FOUNTAIN"); }

ko* mk_fountain(dredge_func func, void* data) {
  fountain * b = (fountain*) mk_ko(sizeof(ctor), f_cleanup, 
      f_view, KO_FOUNTAIN);

  b->dredge = func;
  b->data = data;
  return KO(b);
}
