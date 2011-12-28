#include "ko.h"
#include "kihashmap.h"

size_t next_ko_id(void) {
  static size_t id = 1;
  return id++;
}

static void register_ko(ko* o);
static void unregister_ko(ko* o);

ko* mk_ko(size_t size, cleanup_func cleanup, view_func view, size_t type) {
  ko* o = kmalloc(size);

  o->cleanup = cleanup;
  o->view = view;
  o->type = type;
  o->rc = 1;
  o->id = next_ko_id();

  register_ko(o);
  return o;
}

void _kput(ko* o, const char* file, const char* func) {
  o->rc--; 
  if (o->rc == 1) {
    printk("CLEANUP %s %s : %k ", file, func, o);
    o->rc = 99; // don't let the hashmap delete it too
    unregister_ko(o);
    CLEANUP(o);
    kfree(o);
  }
}

static kihashmap* registry;

void setup_ko_registry() {
  registry = mk_kihashmap(7);
}

static void register_ko(ko* o) {
  kihm_insert(registry, o->id, o);
}

static void unregister_ko(ko* o) {
  kihm_delete(registry, o->id);
}

ko* get_ko(size_t id) {
  return kihm_lookup(registry, id);
}
