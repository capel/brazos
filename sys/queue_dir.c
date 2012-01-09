#include "ko.h"

#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"
#include "../mach.h"

static ko* q_sink(void* p, ko* o) {
  vector* v = (vector*)p;
  vector_push(v, (void*)o);
  kget(o);
  return SINK_ASYNC;
}

static ko* q_pop(void* p) {
  disable_irq(0);
  vector* v = (vector*)p;
  if (v->size == 0) return NULL;

  ko* o = KO(v->data[0]);
  vector_remove(v, 0);
  kput(o);
  enable_irq(0);
  return o;
}

IGET_FUNC(q_size, vector, size);

dir* mk_queue() {
  dir* d = mk_dir();
  vector* v = kmake_vector(sizeof(ko*), UNMANAGED_POINTERS);
  SAFE_ADD(d, mk_sinkhole(q_sink, v), "push");
  SAFE_ADD(d, mk_fountain(q_pop, v), "pop");
  SAFE_ADD(d, mk_window(q_size, v), "size");
  return d;
}
