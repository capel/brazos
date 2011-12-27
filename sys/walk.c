#include "ko.h"

ko* walk(dir* start, const char* path) {
  ko * o;
  vector* v = ksplit_to_vector(path, "/");
  printk("%v", v);
  for (size_t i = 0; i < v->size; i++) {
    o = LOOKUP(start, v->data[i]);
    if (!o) return 0;
    if (IS_BOUND(o)) {
      o = release(o);
    }

    if (i+1 == v->size) {
      return o;
    }

    if (IS_DIR(o)) {
      start = DIR(o);
    } else {
      return 0;
    }
  }

  cleanup_vector(v);
  
  return o; 
}
