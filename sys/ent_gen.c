#include "include/ent_gen.h"
#include "../common/stdlib.h"
#include "include/mem.h"

typedef struct {
  const char* name;
  ent* val;
  perms_t perms;
} vector_de;

#define DE(x) ((vector_de*)(x))
#define VECTOR(e) ((vector*)(e)->d2)

err_t ent_map(ent_lookup* table, size_t size, ent* e, actor* a, const perms_t p,
              size_t *out_size, void** out_ptr) {
  const size_t BUF_SIZE = 2048;
  char* buf = kmalloc(BUF_SIZE); // SUPER magic number for size
  size_t pos = 0;
  vector* v = e->d2;
  for(size_t i = 0; i < size; i++) {
    pos += strlcpy(buf+pos, table[i].name, BUF_SIZE - pos);
  }
  for(size_t i = 0; i < v->size; i++) {
    pos += strlcpy(buf+pos, DE(v->data[i])->name, BUF_SIZE - pos);
  }
  *out_ptr = buf;
  *out_size = pos;
  return SUCCESS;
}

err_t ent_unmap(ent *e, actor* a, void* ptr) {
  kfree(ptr);
}


ent* managed_lookup_func_not_found(ent* e, actor* a, vector* v, size_t level) { 
  return (ent*)E_NOT_FOUND; 
}

ent* vector_find_child(vector* v, const char* name) {
    for(size_t i = 0; i < v->size; i++) {
        if (0 == strcmp(name, DE(v->data[i])->name)) {
            return kget(((vector_de*)v->data[i])->val);
        }
    }
    return (ent*)E_NOT_FOUND;
}

err_t vector_link(ent *e, actor* a, const char* name, 
                          ent* r, const perms_t perms) {
  vector_de* de = kmalloc(sizeof(de));
  de->name = kstrclone(name);
  de->val = kget(r);
  de->perms = perms;
  vector_push(e->d2, de);

  return SUCCESS;
}

void simple_managed_create(ent* e) {
  e->d2 = kmake_vector(sizeof(vector_de*), UNMANAGED_POINTERS);
}

ent* smart_find_child(ent_lookup* table, size_t size, ent* e, actor* a, vector* v, size_t level) {
  const char *name = v->data[level];
  ent* next = vector_find_child(e->d2, name);
  if ((err_t)next != E_NOT_FOUND)  return next;

  for(size_t i = 0; i < size; i++) { 
      if (0 == strcmp(name, table[i].name)) { 
          return table[i].func(e, a, v, level); 
      }
  }
  // Try the default
  return table[0].func(e, a, v, level);
}

ent* walk_lookup(ent* next, vector * v, size_t level) {
  if (IS_ERROR(next)) return next;

  if (level + 1 == v->size) { 
      return next; 
  } else { 
      ent* result = LOOKUP(next, v, level + 1); 
      kput(next);
      return result;
  }
}

ent* smart_managed_lookup(ent_lookup* table, size_t size,
                      ent* e, actor* a, vector* v, size_t level) {
  ent * next = smart_find_child(table, size, e, a, v, level);
  return walk_lookup(next, v, level);
}

ent* simple_managed_lookup(ent * e, actor* a, vector* v, size_t level) {
  ent* next = vector_find_child(e->d2, v->data[level]);
  return walk_lookup(next, v, level);
}

err_t simple_managed_link(ent* e, actor* a, ent* child, perms_t perms, vector * v, size_t level) {
  ent* child = vector_find_child(e->d2, v->data[level]);

  if (level + 2 == v->size) { 
    if (child == E_NOT_FOUND) {
      return vector_link(e, a, v->data[level], child, perms);
    } else {
      kput(child);
      return E_ALREADY_EXISTS;
    }
  } else {
      err_t result = LINK(child, a, r, perms, v, level + 1); 
      kput(child);
      return result;
  }
}

ent* not_found_lookup(ent* e, actor* a, vector* v, size_t level) {
  return E_NOT_FOUND;
}

ent* not_found_lookup(ent* e, actor* a, vector* v, size_t level) {
  return E_NOT_SUPPORTED;
}


err_t disable_link(ent* e, actor* a, const char* path, ent* r, 
    const perms_t perms) {
  return E_NOT_SUPPORTED;
}

err_t disable_unlink(ent* e, actor* a, const char* path) {
  return E_NOT_SUPPORTED;
}

err_t disable_map(ent* e, actor* a, const perms_t requested_perms, 
        size_t* out_size, void** out_ptr) {
  return E_NOT_SUPPORTED;
}

err_t disable_unmap(ent* e, actor* a) {
  return E_NOT_SUPPORTED;
}


