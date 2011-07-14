#include "include/common.h"

static err_t map(string * s, size_t *out_size, void **out_ptr) {
  *out_size = (size_t)s->d2;
  *out_ptr = s->d1;
  return SUCCESS;
}

static err_t unmap(string *s, void* ptr) {}

static void cleanup(string * s) {
  entdealloc(s);
}

static ent_funcs integer_funcs = {
  .lookup = disable_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = map,
  .unmap = unmap,
  .cleanup = cleanup
};

integer * mkint(int i) {
  integer *i = entalloc(&integer_funcs);
  s->d1 = (void*)i;
  return i;
}
