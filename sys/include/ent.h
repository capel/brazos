#ifndef ENTS_H
#define ENTS_H


#include "../../common/types.h"

// Range 0 < err_t < 4096
typedef size_t err_t;
// Range 4096 <= rid_t < SIZE_T_MAX
typedef size_t rid_t;
// * P_R Read permission.
// * P_COW Can read and write, but writing creates a new copy.
// * P_W Can read from and write to the original.
// Note that permissions are strictly increasing in size as
// they become more permissive, so they can be compared like
// integers.
typedef size_t perms_t;

// Permission defines
#define P_R 0xf001
#define P_COW 0xf002
#define P_W 0xf003

#include "../../common/errors.h"


typedef struct _ent {
    size_t rc;
    void* d1; // Private data
    void* d2; // Private data or, if using ent_gen.h, ent_gen private data.
    struct _ent_funcs* f;
} ent;

#include "actor.h"
#include "kvector.h"

typedef ent* (*lookup_func)(ent* e, actor* a, const vector* v, size_t level);
typedef err_t (*link_func)(ent* e, actor* a, ent* child,
                          const vector* v, size_t level);
typedef err_t (*unlink_func)(ent* e, actor* a, const char* path);
typedef err_t (*map_func)(ent* e, actor* a, const perms_t requested_perms, 
        size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(ent* e, actor* a, void* ptr);
typedef void (*cleanup_func)(ent* e);

typedef struct _ent_funcs {
    lookup_func lookup;
    link_func link;
    unlink_func unlink;
    map_func map;
    unmap_func unmap;
    cleanup_func cleanup;
} ent_funcs;

#define LOOKUP(e, a, v, level) ((e)->f->lookup((e), (a), (v), (level)))
#define LINK(e, a, child, v, level) ((e)->f->link((e), (a), (child), (v), (level)))
#define UNLINK(e, a, path) ((e)->f->unlink((e), (a), (path)))
#define MAP(e, a, p, o, ptr) ((e)->f->map((e), (a), (p), (o), (ptr)))
#define UNMAP(e, a, p) ((e)->f->unmap((e), (a), (p)))
#define CLEANUP(e) ((e)->f->cleanup((e)))

ent* LOOKUP_R(ent* e, actor* a, const char* path);
err_t LINK_R(ent* e, actor* a, ent* child, const char* path);
err_t MAP_R(ent* e, actor* a, const perms_t rp, size_t *out_size, void** out_ptr);


ent* entalloc(ent_funcs* f);
void entdealloc(ent* e);

ent* kget(ent* e);
void kput(ent* e);



#endif
