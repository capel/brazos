#ifndef ENTS_H
#define ENTS_H


#include "../../common/types.h"
#include "../../common/errors.h"
#include "../../common/syscalls.h"


typedef struct _ent {
    size_t rc;
    void* d1; // Private data
    void* d2; // Private data or, if using ent_gen.h, ent_gen private data.
    struct _ent_funcs* f;
} ent;
typedef ent string;

typedef ent* (*lookup_func)(ent* e, const vector* v, size_t level);
typedef err_t (*link_func)(ent* e, ent* child, const vector* v, size_t level);
typedef err_t (*unlink_func)(ent* e, const char* path);
typedef err_t (*map_func)(ent* e, size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(ent* e, void* ptr);
typedef void (*cleanup_func)(ent* e);
typedef string* (*list_func)(ent* e);

typedef struct _ent_funcs {
    lookup_func lookup;
    link_func link;
    unlink_func unlink;
    map_func map;
    unmap_func unmap;
    cleanup_func cleanup;
    list_func list;
} ent_funcs;

#define LOOKUP(e, v, level) ((e)->f->lookup((e), (v), (level)))
#define LINK(e, child, v, level) ((e)->f->link((e), (child), (v), (level)))
#define UNLINK(e, path) ((e)->f->unlink((e), (path)))
#define MAP(e, o, ptr) ((e)->f->map((e), (o), (ptr)))
#define UNMAP(e, p) ((e)->f->unmap((e), (p)))
#define CLEANUP(e) ((e)->f->cleanup((e)))
#define LIST(e) ((e)->f->list((e)))

ent* LOOKUP_R(ent* e, const char* path, ...);
ent* root(void);
#define FS(path, ...) LOOKUP_R(root(), path, ## __VA_ARGS__)

err_t LINK_R(ent* e, ent* child, const char* path, ...);
#define LINK_FS(e, path, ...) \
  LINK_R(root(), (e), path, ## __VA_ARGS__)

ent* entalloc(ent_funcs* f);
void entdealloc(ent* e);

ent* kget(ent* e);
void kput(ent* e);



#endif
