#ifndef ENT_GEN_H
#define ENT_GEN_H

#include "ent.h"

typedef ent* (*managed_lookup_func)(ent* e, const vector* v, size_t level, bool *done);
ent* managed_lookup_func_not_found(ent* e, const vector* v, size_t level, bool *done);

typedef struct {
    const char* name;
    managed_lookup_func func;
} ent_lookup;

typedef struct {
  const char* name;
  ent* val;
} vector_de;

#define VECTOR_ENT(de) 
#define GET_ENT(de) (((vector_de*)(de))->val)
#define GET_NAME(de) (((vector_de*)(de))->name)

#define DEFAULT_LOOKUP_FUNC 0

ent* smart_managed_lookup(ent_lookup* table, size_t size,
                      ent* e, const vector* v, size_t level);    
err_t ent_map(ent_lookup* table, size_t size, ent* e,
              size_t *out_size, void** out_ptr);
err_t ent_unmap(ent *e, void* ptr);

#define NAME_LOOKUP(_data_) _gen_managed_lookup_ ## _data
#define MAKE_LOOKUP(_data_) \
  static ent* NAME_LOOKUP(_data_) (ent * e, \
            const vector* v, size_t level) { \
        return smart_managed_lookup(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, v, level); } 

#define NAME_MAP(_data_) _gen_map ## _data_
#define MAKE_MAP(_data_) \
  static err_t NAME_MAP(_data_) (ent * e, \
            size_t *out_size, void** out_ptr) { \
        return ent_map(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, out_size, out_ptr); }


#define NAME_UNMAP(_data_) _gen_unmap ## _data_
#define MAKE_UNMAP(_data_) \
  static err_t NAME_UNMAP(_data_) (ent * e, void* ptr) { \
        return ent_unmap(e, ptr); }

ent* simple_managed_lookup(ent* e, const vector* v, size_t level);
err_t simple_managed_link(ent *e, ent* r, const vector* v, size_t level);
void simple_managed_create(ent* e);

err_t vector_link(ent * e, ent *r,  const char* name);

ent* not_found_lookup(ent* e, const vector* v, size_t level);
ent* disable_lookup(ent* e, const vector* v, size_t level);

err_t disable_link(ent* e, ent* child, const vector* v, size_t level);

err_t disable_unlink(ent* e, const char* path);

err_t disable_map(ent* e, size_t* out_size, void** out_ptr);

err_t disable_unmap(ent* e, void* ptr);
string* disable_list(ent* e);

#define ONLY_CLEANUP(name, _cleanup) \
  static ent_funcs name = { \
    .lookup = disable_lookup,\
    .link = disable_link, \
    .unlink = disable_unlink, \
    .map = disable_map, \
    .unmap = disable_unmap, \
    .cleanup = _cleanup \
  }


#endif
