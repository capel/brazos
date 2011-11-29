#include "ko.h"
#include "../stdlib.h"
#include "../mem.h"

typedef struct {
  ko o;
  bound_func func;
  void* data;
} bound;

ko* bind0(bound_func func, void* data) {
  bound * b = kmalloc(sizeof(bound));
  b->o.type = KO_BOUND;
  b->o.rc = 1;
  b->o.v = 0;

  b->func = func;
  b->data = data;
  return (ko*)b;
}

ko* release0(ko* o) {
  assert(o->type = KO_BOUND);
  bound * b = (bound*) o;
  printk("bound: %p, func %p, obj %p", b, b->func, b->data);
  return b->func(b->data);
}

