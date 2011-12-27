#include "ko.h"

#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"


static ko* q_sink(vector* v, ko* o) {
  vector_push(v, (void*)o);
  kget(o);
  return SINK_ASYNC;
}

static ko* q_pop(vector* v) {
  if (v->size == 0) return NULL;

  ko* o = KO(v->data[0]);
  vector_remove(v, 0);
  kput(o);
  return o;
}

static ko* q_size(vector* v) {
  char buf[32];
  utoa(buf, 32, v->size);
  return mk_msg(buf);
}

dir* mk_queue() {
  dir* d = mk_dir();
  vector* v = kmake_vector(sizeof(ko*), UNMANAGED_POINTERS);
  SAFE_ADD(d, MK_SINKHOLE(q_sink, v), "push@");
  printk("done making push");
  SAFE_ADD(d, BIND(q_pop, v), "pop^");
  SAFE_ADD(d, BIND(q_size, v), "size");
  return d;
}
