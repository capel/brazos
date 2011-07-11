#ifndef KVECTOR_H
#define KVECTOR_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "mem.h"
#include "../../common/vector.h"

extern alloc_funcs kernel_alloc_funcs;

static inline vector* kmake_vector(int data_type_size, enum cleanup_type type) {
    return _make_vector(data_type_size, type, &kernel_alloc_funcs);
}
static inline vector* ksplit_to_vector(const char * str, const char* seps) {
    return _split_to_vector(str, seps, &kernel_alloc_funcs);
}

#endif
