#ifndef USER_VECTOR_H
#define USER_VECTOR_H
#include "malloc.h"
#include "../common/vector.h"

static inline vector* make_vector(int data_type_size, enum cleanup_type type)
{
    return _make_vector(data_type_size, type, &user_alloc_funcs);
}

static inline vector* split_to_vector(const char * str, const char* seps)
{
    return _split_to_vector(str, seps, &user_alloc_funcs);
}

#endif
