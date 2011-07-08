#ifndef VFS_H
#define VFS_H

#include "kfs.h"

#define EMPTY_DE {"", 0, KFS_NORMAL_FILE}

typedef kfile* (*lookup_func)(kfile* f, const char * name);

typedef struct _lookup_entry {
    const char* name;
    lookup_func func;
} lookup_entry;

#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof(*array))
#define LOOKUP_FUNC(x) static kfile* x(kfile* f, const char* name) 
#define DEFAULT_LOOKUP_FUNC 0

#define LOOKUP_FUNC_FACTORY_NAME(_name_) _gen_lookup_func_##_name_

#define LOOKUP_FUNC_FACTORY(_name_, _entries_) \
static kfile* LOOKUP_FUNC_FACTORY_NAME(_name_) \
            (kfile* f, vector * v, size_t level) { \
    lookup_entry *entries = _entries_; \
    const char* name = v->data[level]; \
    lookup_func default_lookup = NULL; \
    kfile* next = NULL; \
\
    for(size_t i = 0; i < (SIZEOF_ARRAY(_entries_)); i++) { \
        if (entries[i].name == DEFAULT_LOOKUP_FUNC) { \
            default_lookup = entries[i].func; \
        } else if (0 == strcmp(name, entries[i].name)) { \
            next = entries[i].func(f, name); \
            goto cleanup; \
        } \
    } \
    /* No matches, try the default if we can. */ \
    if (default_lookup == NULL) { \
        kput_file(f); \
        return NULL; \
    } else { \
        next = default_lookup(f, name); \
        goto cleanup; \
    } \
 \
cleanup: \
 \
    kput_file(f); \
    if (!next) return NULL; \
 \
    if (level + 1 == v->size) { \
        return next; \
    } else { \
        return next->lookup_file(next, v, level + 1); \
    } \
}

#endif
