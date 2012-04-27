#include "include/ent.h"
#include "include/kvector.h"
#include "include/kfs.h"
#include "include/common.h"

ent* entalloc(ent_funcs *funcs) {
  ent* e = kmalloc(sizeof(ent));
  e->f = funcs;
  e->rc = 1;
  return e;
}

void entdealloc(ent* e) {
  kfree(e);
}

ent* LOOKUP_R(ent* e, const char* fmt, ...) {
  char buf[128];

  va_list va;
  va_start(va, fmt);
  vprintf(buf, 128, fmt, va, true);
  va_end(va);

  vector* v = ksplit_to_vector(buf, "/");
  ent * ret = LOOKUP(e, v, 0);
  printk("%d", ret);
  cleanup_vector(v);
  return ret;
}

err_t LINK_R(ent* e, ent* child, const char* fmt, ...) {
   char buf[128];

  va_list va;
  va_start(va, fmt);
  vprintf(buf, 128, fmt, va, true);
  va_end(va);
  
  vector* v = ksplit_to_vector(fmt, "/");
  err_t ret = LINK(e, child, v, 0);
  printk("%d", ret);
  cleanup_vector(v);
  return ret;
}

ent* kget(ent* e) {
  e->rc++;
  return e;
}
void kput(ent* e) {
  e->rc--;
  if (e->rc == 0) {
    CLEANUP(e);
  }
}

