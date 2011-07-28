#include "include/common.h"
#include "include/ent_gen.h"
  
static void cleanup(ent* e) { printk("TODO"); }

static ent_funcs dir_funcs = {
  .lookup = simple_managed_lookup,
  .link = simple_managed_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = cleanup
};

ent* mkdir() {
  ent* dir = entalloc(&dir_funcs);
  simple_managed_create(dir);
  return dir;
}

static dir* _root;
dir* root() {
    return _root;
}

void mkroot() {
    _root = mkdir();
    kget(_root); // it contains itself!
}

