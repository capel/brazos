#include "../common/stdlib.h"
#include "include/ent.h"
#include "include/kvector.h"
#include "include/ent_gen.h"
#include "include/kfs.h"

ent* kf_lookup_path(actor *a, const char* path, ent *start) {
    assert(start);
    
    if (0 == strcmp(path, "/")) return root();

    vector *v;

    if (path[0] == '/') {
      start = root();
      v = ksplit_to_vector(path, "/");
    } else {
      v = ksplit_to_vector(path, "/");
    }

    ent* e = LOOKUP(start, a, v, 0);
    cleanup_vector(v);
    return e;
}

static void cleanup(ent* e) {}

static ent_funcs dir_funcs = {
  .lookup = simple_managed_lookup,
  .link = simple_managed_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = cleanup
};

ent* kcreate_dir() {
  ent* dir = entalloc(&dir_funcs);
  simple_managed_create(dir);
  return dir;
}



