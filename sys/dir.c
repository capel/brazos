#include "ko.h"
#include "khashmap.h"

typedef struct dir_impl {
  dir d;
  khashmap * h;
} dir_impl;

static ko* dir_lookup(dir* d, const char** path) {
  dir_impl * di = (dir_impl*) d;

  assert(path);
  if (!*path) {
    return KO(d);
  }

  ko* child = khm_lookup(di->h, *path);
  if (!child) {
    return NULL;
  }
  
  if (IS_BOUND(child)) {
    child = release(child);
  }

  if (IS_DIR(child)) {
    return LOOKUP(DIR(child), path+1);
  } else {
    // its chill, no more path left anyway
    if (!path[1]) return KO(child);

    // More path but its not a dir...
    return 0;
  }
}

static err_t dir_link(dir* d, ko* child, const char* name) {
  dir_impl * di = (dir_impl*)d;

  bool success = khm_insert(di->h, name, child);
  if (!success) {
    khm_delete(di->h, name);
    khm_insert(di->h, name, child);
  }
  return 0;
}

static err_t dir_unlink(dir* d, const char* name) {
  dir_impl * di = (dir_impl*) d;
  return khm_delete(di->h, name) ? 0 : E_BAD_FILENAME;
}

static void dir_cleanup(ko* o) {
  dir_impl * di = (dir_impl*) o;
  
  khm_cleanup(di->h);
}

static err_t dir_map(file* f, size_t* out_size, void** out_ptr) {
  dir_impl * d = (dir_impl*)f;
  printk("%h", d->h);

  vector* v = khm_keys(d->h);
  const char* s = vector_join(v, "/");

  *out_ptr = (void*)s;
  *out_size = strlen(s);
  return 0;
}

static err_t dir_unmap(file* f, void* ptr) {
  kfree(ptr);
  return 0;
}

static dir_vtable dir_vt = {
  .lookup = dir_lookup,
  .link = dir_link,
  .unlink = dir_unlink,
};

static file_vtable dir_file_vt = {
  .map = dir_map,
  .unmap = dir_unmap,
};


dir* mk_dir() {
  dir_impl* d = kmalloc(sizeof(dir_impl));

  KO(d)->cleanup = dir_cleanup;
  KO(d)->type = KO_DIR;
  KO(d)->rc = 1;
  FILE(d)->v = &dir_file_vt;
  DIR(d)->v = &dir_vt;

  d->h = mk_khashmap(3);

  return (dir*)d;
}



