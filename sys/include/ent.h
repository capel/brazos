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

#include "kvector.h"

typedef ent* (*lookup_func)(ent* e, const vector* v, size_t level);
typedef err_t (*link_func)(ent* e, ent* child, const vector* v, size_t level);
typedef err_t (*unlink_func)(ent* e, const char* path);
typedef err_t (*map_func)(ent* e, perms_t requested_perms, 
        size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(ent* e, void* ptr);
typedef void (*cleanup_func)(ent* e);

typedef struct _ent_funcs {
    lookup_func lookup;
    link_func link;
    unlink_func unlink;
    map_func map;
    unmap_func unmap;
    cleanup_func cleanup;
} ent_funcs;

#define LOOKUP(e, v, level) ((e)->f->lookup((e), (v), (level)))
#define LINK(e, child, v, level) ((e)->f->link((e), (child), (v), (level)))
#define UNLINK(e, path) ((e)->f->unlink((e), (path)))
#define MAP(e, p, o, ptr) ((e)->f->map((e), (p), (o), (ptr)))
#define UNMAP(e, p) ((e)->f->unmap((e), (p)))
#define CLEANUP(e) ((e)->f->cleanup((e)))

ent* LOOKUP_R(ent* e, const char* path, ...);
ent* root(void);
#define FS(path, ...) LOOKUP_R(root(), path, ## __VA_ARGS__)

err_t LINK_R(ent* e, ent* child, const char* path, ...);


ent* entalloc(ent_funcs* f);
void entdealloc(ent* e);

ent* kget(ent* e);
void kput(ent* e);



#endif
