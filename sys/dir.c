#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"
#include "dir.h"

err_t void_unmap(ko* o, void *ptr) { return 0; }

ko* no_lookup(ko* o, char** path) {
  return 0;
}

err_t no_link(ko* o, ko* child, const char* name) {
  return E_NOT_SUPPORTED;
}

err_t no_unlink(ko* o, void* ptr) {
  return E_NOT_SUPPORTED;
}

static ko* dir_lookup(dir* d, const char** path) {
  assert(path);
  if (!*path) {
    return (ko*)d;
  }

  ko* child = khm_lookup(d->h, *path);
  if (!child) {
    return NULL;
  }
  
  if (IS_BOUND(child)) {
    child = release0(child);
  }

  return LOOKUP(child, path+1);
}

static err_t dir_link(dir* d, ko* child, const char* name) {
  bool success = khm_insert(d->h, name, child);
  if (!success) {
    printk("Detected failure, delete and insert");
    khm_delete(d->h, name);
    khm_insert(d->h, name, child);
  }
  return 0;
}

static err_t dir_unlink(dir* d, const char* name) {
  return khm_delete(d->h, name) ? 0 : E_BAD_FILENAME;
}

static void dir_cleanup(dir* d) {
  khm_cleanup(d->h);
  kfree(d);
}

static err_t dir_map(dir* d, size_t* out_size, void** out_ptr) {
  vector* v = khm_keys(d->h);
  const char* s = vector_join(v, "/");

  *out_ptr = (void*)s;
  *out_size = strlen(s);
  return 0;
}

static err_t dir_unmap(dir* d, void* ptr) {
  kfree(ptr);
  return 0;
}

static vtable dir_vt = {
  .lookup = (lookup_func)dir_lookup,
  .link = (link_func)dir_link,
  .unlink = (unlink_func)dir_unlink,
  .map = (map_func)dir_map,
  .unmap = (unmap_func)dir_unmap,
  .cleanup = (cleanup_func)dir_cleanup,
};


ko* mk_dir() {
  dir* d = kmalloc(sizeof(dir));
  d->o.type = KO_OBJ;
  d->o.v = &dir_vt;
  d->o.rc = 1;

  d->h = mk_khashmap(3);
  return (ko*)d;
}



