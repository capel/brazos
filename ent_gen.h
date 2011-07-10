#ifndef ENT_GEN_H
#define ENT_GEN_H

#include "ent.h"

#define NAME_CLOSURE_LOOKUP(_func_, _data_) _gen_ ## _func_ ## _data
#define MAKE_CLOSURE_LOOKUP(_func_, _data_) \
    static err_t NAME_CLOSURE_LOOKUP(_func_, _data_) (ent * e, actor* a, \
            const char * path, ent **out) { \
        return _func_(_data_, sizeof(_data_)/sizeof(_data_[0]), \
                e, a, path, out); } 

ent* vector_ent_lookup(kvector* v, const char* path) {
    for(size_t i = 0; i < v->size; i++) {
        if (0 == strcmp(path, v->data[i]->name)) {
            return v->data[i]->val;
        }
    }
    return NULL;
}


err_t ent_lookup_table(ent_lookup* table, size_t size,
                      ent* e, actor* a, const char* path, ent **out) {
    *out = vector_ent_lookup(e->d2, path);
    if (*out) return SUCCESS;

    for (size_t i = 1; i < size; i++) {
        if (0 == strcmp(path, table[i].name)) {
            return table[i].f(e, a, path, out);
        }
    }
    return table[i].f(e, a, path, out);
}
    




#endif
