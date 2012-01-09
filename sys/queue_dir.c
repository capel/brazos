#include "ko.h"

#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"
#include "../mach.h"

typedef struct {
  vector* q;
  vector* futures;
} qdir;

static ko* q_sink(void* p, ko* o) {
  qdir* q = (qdir*)p;

  if (q->futures->size > 0) {
    future * f = vector_pop_front(q->futures);
    RESOLVE(f, o);
    return SINK_ASYNC;
  }

  vector_push(q->q, (void*)o);
  kget(o);
  return SINK_ASYNC;
}

static ko* q_pop(void* p) {
  qdir* q = (qdir*)p;
  if (q->q->size == 0) {
    future* f = mk_future();
    vector_push(q->futures, (void*)f);
    return KO(f);
  }

  ko* o = KO(vector_pop_front(q->q));

  return o;
}

IGET_FUNC(q_size, qdir, q->size);

dir* mk_queue() {
  dir* d = mk_dir();
  qdir* q = kmalloc(sizeof(qdir));
  
  q->q = kmake_vector(sizeof(ko*), UNMANAGED_POINTERS);
  q->futures = kmake_vector(sizeof(ko*), UNMANAGED_POINTERS);
  SAFE_ADD(d, mk_sinkhole(q_sink, q), "push");
  SAFE_ADD(d, mk_fountain(q_pop, q), "pop");
  SAFE_ADD(d, mk_window(q_size, q), "size");
  return d;
}
