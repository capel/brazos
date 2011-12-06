#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"
#include "dir.h"

err_t void_unmap(ko* o, void *ptr) { return 0; }
const char* vector_join(vector* v, char joiner);

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
  printk("path %s", *path);
  assert(path);
  if (!*path) {
    printk("Not path");
    return (ko*)d;
  }
  printk("*path %s", *path);

  ko* child = khm_lookup(d->h, *path);
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

static err_t dir_link(dir* d, ko* child, const char* name) {
  return khm_insert(d->h, name, child) ? 0 : E_ERROR;
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

  printk("keys():");
  print_vector(v, "%s", 0);
  printk("/keys()");
  
  const char* s = vector_join(v, '/');

  *out_ptr = (void*)s;
  *out_size = strlen(s);
  return 0;
}


const char* vector_join(vector* v, char joiner) {
  size_t needed = 0;
  size_t pos = 0;
  for(size_t i = 0; i < v->size; i++) {
    needed += strlen(v->data[i]);
  }

  char* s = kmalloc(needed + 1);

  for(size_t i = 0; i < v->size; i++) {
    size_t len = strlen(v->data[i]);
    memcpy(s + pos, v->data[i], len);
    pos += len;
    s[pos++] = '/';
  }
  s[pos-1] = '\0';
  return s;
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
  printk("returning dir %p", d);
  return (ko*)d;
}



