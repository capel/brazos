#include "include/common.h"

static err_t map(string * s, perms_t rq, size_t *out_size, void **out_ptr) {
  *out_size = (size_t)s->d2;
  *out_ptr = s->d1;
  return SUCCESS;
}

static err_t unmap(string *s, void* ptr) { return SUCCESS; }

static void cleanup(string * s) {
  kfree(s->d1);
  entdealloc(s);
}

static ent_funcs string_funcs = {
  .lookup = disable_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = map,
  .unmap = unmap,
  .list = disable_list,
  .cleanup = cleanup
};

string * mkstring(const char* raw) {
  string *s = entalloc(&string_funcs);
  s->d2 = (void*)strlen(raw);
  s->d1 = (void*)kstrclone(raw);
  return s;
}
