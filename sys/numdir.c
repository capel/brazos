#include "ko.h"

static ko* dir_lookup(numdir* d, const char** path) {
  printk("path %s", *path);
  assert(path);
  if (!*path) {
    printk("Not path");
    return (ko*)d;
  }
  printk("*path %s", *path);

  ko* child = kihm_lookup(d->h, atoi(*path));
  printk("child %p", child);
  if (!child) {
    printk("No child");
    return NULL;
  }
  
  if (IS_BOUND(child)) {
    printk("Is bound!");
    child = release0(child);
    printk("child bound %p", child);
  }

  return LOOKUP(child, path+1);
}

static err_t dir_link(numdir* d, ko* child, const char* name) {
  unsigned i = atoi(name);
  if (!i) return E_BAD_FILENAME;
  return kihm_insert(d->h, i, child) ? 0 : E_ERROR;
}

static err_t dir_unlink(numdir* d, const char* name) {
  unsigned i = atoi(name);
  if (!i) return E_BAD_FILENAME;
  return kihm_delete(d->h, i) ? 0 : E_BAD_FILENAME;
}

static void dir_cleanup(numdir* d) {
  kihm_cleanup(d->h);
  kfree(d);
}

static err_t dir_map(numdir* d, size_t* out_size, void** out_ptr) {
  vector* vi = kihm_keys(d->h);
  vector* v = kmake_vector(sizeof(const char*), MANAGED_POINTERS);
  for(size_t i = 0; i < v->size; i++) {
    const char* a = kitoa((unsigned)v->data[i]);
    vector_push(v, (char*)a);
  }

  printk("keys():");
  print_vector(v, "%s", 0);
  
  const char* s = vector_join(v, "/");

  *out_ptr = (void*)s;
  *out_size = strlen(s);

  cleanup_vector(vi);
  cleanup_vector(v);
  return 0;
}

static err_t dir_unmap(numdir* d, void* ptr) {
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


ko* mk_numdir() {
  numdir* d = kmalloc(sizeof(numdir));
  d->o.type = KO_OBJ;
  d->o.v = &dir_vt;
  d->o.rc = 1;

  d->h = mk_kihashmap(3);
  printk("returning numdir %p", d);
  return (ko*)d;
}



