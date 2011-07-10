#ifndef ENT_H
#define ENT_H



// Error type. Range: 0 < err_t < 4096
// Note: this can and is returned in place of both rid_t and void*.
// Both other types will never be in this range, so
// there is no ambiguity.
typedef size_t err_t
// Resource id type. Always can instead be an err_t.
// Range: 4096 <= rid_t < SIZE_T_MAX
typedef size_t rid_t
// Permissions type. It is one of:
// * P_R Read permission.
// * P_COW Can read and write, but writing creates a new copy.
// * P_W Can read from and write to the original.
// Note that permissions are strictly increasing in size as
// they become more permissive, so they can be compared like
// integers.
typedef size_t perms_t

// Permission defines
#define P_R 0xf001
#define P_COW 0xf002
#define P_W 0xf003

#include "user/error.h"

typedef struct _ent {
    size_t rc;
    void* d1; // Private data
    void* d2; // Private data or, if using ent_gen.h, ent_gen private data.
    struct _ent_funcs* f;
} ent;

typedef err_t (*lookup_func)(ent* e, actor* a, const char* path, ent **out);
typedef err_t (*link_func)(ent* e, actor* a, const char* path,
        ent* r, const perms_t perms);
typedef err_t (*unlink_func)(ent* e, actor* a, const char* path);
typedef err_t (*map_func)(ent* e, actor* a, const perms_t requested_perms, 
        size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(ent* e, actor* a);

typedef struct _ent_funcs {
    lookup_func lookup;
    link_func link;
    unlink_func unlink;
    map_func map;
    unmap_func unmap;
} ent_funcs;

#endif
