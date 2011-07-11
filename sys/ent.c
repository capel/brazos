#include "include/ent.h"
#include "include/kvector.h"

ent* entalloc(ent_funcs funcs) {
  ent* e = kmalloc(sizeof(ko));
  e->funcs = funcs;
  e->rc = 1;
  return e;
}

void entdealloc(ent* e) {
  kfree(e);
}

ent* LOOKUP_R(ent* e, actor* a, const char* path) {
  vector* v = ksplit_to_vector(path, "/");
  ent * ret = LOOKUP(e, a, v, 0);
  cleanup_vector(v);
  return ret;
}

err_t LINK_R(ent* e, actor* a, ent* child, const char* path) {
  vector* v = ksplit_to_vector(path, "/");
  err_t ret = LINK(e, a, child, v, 0);
  cleanup_vector(v);
  return ret;
}

err_t MAP_R(ent* e, actor* a, const perms_t rp, size_t *out_size, void** out_ptr) {
  vector* v = ksplit_to_vector(path, "/");
  err_t ret = MAP(e, a, rp, v, 0, out_size, out_ptr);
  cleanup_vector(v);
  return ret;
}

void kget(ent* e) {
  e->rc++;
  return e;
}
void kput(ent* e) {
  e->rc--;
  if (e->rc == 0) {
    CLEANUP(e);
  }
}
