#include "stdlib.h"
#include "kfs.h"
#include "kvector.h"
#include "ent_gen.h"

ko* kf_lookup_path(const char* path, kfile *start) {
    assert(start);
    assert(start->type == KFS_DIR);
    
    if (0 == strcmp(path, "/")) return root();

    kvector *v;

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

static dir_funcs = {
  .lookup = simple_managed_lookup,
  .link = simple_managed_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = disable_cleanup
};

void kcreate_dir() {
  ent* dir = entalloc(dir_funcs);
  simple_managed_create(dir);
  return dir;
}

