#ifndef ENT_GEN_H
#define ENT_GEN_H

#include "ent.h"

typedef struct {
    const char* name;
    lookup_func func;
} ent_lookup;


#define NAME_CLOSURE_LOOKUP(_func_, _data_) _gen_ ## _func_ ## _data
#define MAKE_CLOSURE_LOOKUP(_func_, _data_) \
    static ent* NAME_CLOSURE_LOOKUP(_func_, _data_) (ent * e, actor* a, \
            kvector* v, size_t level) { \
        return _func_(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, a, v, level); } 

ent* ent_lookup_table(ent_lookup* table, size_t size,
                      ent* e, actor* a, kvector* v, size_t level);    

ent* simple_managed_lookup(ent* e, actor* a, vector* v, size_t level);
err_t simple_managed_link(ent *e, actor* a, ent* r, const perms_t p, 
    const vector* v, const size_t level);
void simple_managed_create(ent* e);

ent* not_found_lookup(ent* e, actor* a, vector* v, size_t level);
ent* disable_lookup(ent* e, actor* a, vector* v, size_t level);

err_t disable_link(ent* e, actor* a, const char* path, ent* r, 
    const perms_t perms);

err_t disable_unlink(ent* e, actor* a, const char* path);

err_t disable_map(ent* e, actor* a, const perms_t requested_perms, 
        size_t* out_size, void** out_ptr);

err_t disable_unmap(ent* e, actor* a);

#endif
