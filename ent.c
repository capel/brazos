#include "ent.h"

ent* entalloc(ent_funcs funcs) {
  ent* e = kmalloc(sizeof(ko));
  e->funcs = funcs;
  e->rc = 1;
  return e;
}

void entdealloc(ent* e) {
  kfree(e);
}
