#ifndef ENT_GEN_H
#define ENT_GEN_H

#include "ent.h"

typedef ent* (*managed_lookup_func)(ent* e, actor* a, vector* v, size_t level);
ent* managed_lookup_func_not_found(ent* e, actor* a, vector* v, size_t level);

typedef struct {
    const char* name;
    managed_lookup_func func;
} ent_lookup;

#define DEFAULT_LOOKUP_FUNC 0

ent* ent_lookup_table(ent_lookup* table, size_t size,
                      ent* e, actor* a, const vector* v, size_t level);    
err_t ent_map(ent_lookup* table, size_t size, ent* e, actor* a, const perms_t p,
              size_t *out_size, void** out_ptr);
err_t ent_unmap(ent *e, actor* a, void* ptr);

#define NAME_LOOKUP(_data_) _gen_managed_lookup_ ## _data
#define MAKE_LOOKUP(_data_) \
  static ent* NAME_LOOKUP(_data_) (ent * e, actor* a, \
            const vector* v, size_t level) { \
        return ent_lookup_table(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, a, v, level); } 

#define NAME_MAP(_data_) _gen_map ## _data_
#define MAKE_MAP(_data_) \
  static err_t NAME_MAP(_data_) (ent * e, actor* a, \
            const perms_t p, size_t *out_size, void** out_ptr) { \
        return ent_map(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, a, p, out_size, out_ptr); }


#define NAME_UNMAP(_data_) _gen_unmap ## _data_
#define MAKE_UNMAP(_data_) \
  static err_t NAME_UNMAP(_data_) (ent * e, actor* a, void* ptr) { \
        return ent_unmap(e, a, ptr); }

ent* simple_managed_lookup(ent* e, actor* a, const vector* v, size_t level);
err_t simple_managed_link(ent *e, actor* a, ent* r, const vector* v, const size_t level);
void simple_managed_create(ent* e);


ent* not_found_lookup(ent* e, actor* a, const vector* v, size_t level);
ent* disable_lookup(ent* e, actor* a, const vector* v, size_t level);

err_t disable_link(ent* e, actor* a, ent* child, const vector* v, size_t level);

err_t disable_unlink(ent* e, actor* a, const char* path);

err_t disable_map(ent* e, actor* a, const perms_t requested_perms, 
        size_t* out_size, void** out_ptr);

err_t disable_unmap(ent* e, actor* a, void* ptr);

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
