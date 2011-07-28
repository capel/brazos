#include "include/common.h"

static err_t map(integer * i, perms_t rp, size_t *out_size, void **out_ptr) {
  *out_size = sizeof(int);
  *out_ptr = &i->d1;
  return SUCCESS;
}

static err_t unmap(integer *i, void* ptr) { return SUCCESS; }

static void cleanup(integer * i) {
  entdealloc(i);
}

static ent_funcs i_funcs = {
  .lookup = disable_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = map,
  .unmap = unmap,
  .cleanup = cleanup
};

integer * mkint(int raw) {
  integer *i = entalloc(&i_funcs);
  i->d1 = (void*)raw;
  return i;
}
