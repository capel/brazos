#include "include/actor.h"

actor* KERNEL;

actor_funcs kfuncs = {
  .add_mapping = add,
  .remove_mapping = remove,
  .lookup_rid = lookup
};


actor* kcreate_actor(size_t type) {
  actor* a = kmalloc(sizeof(actor));
  a->f = kfuncs;
  a->aid = type;

  return a;
}
