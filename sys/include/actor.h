#ifndef ACTOR_H
#define ACTOR_H

#define ACTOR_KERNEL 1
#define ACTOR_USER 2

struct _ent;

typedef struct {
    size_t rc;
    size_t aid;
    void* d2;
    struct _actor_funcs* f;
} actor;

typedef err_t (*add_mapping_func)(actor* a, const void* ptr, 
               const size_t size, const perms_t perms);
typedef err_t (*remove_mapping_func)(actor* a, struct _ent* e);
typedef struct _ent* (*lookup_rid_func)(actor* a, rid_t rid);

typedef struct _actor_funcs {
    add_mapping_func add_mapping;
    remove_mapping_func remove_mapping;
    lookup_rid_func lookup_rid;
} actor_funcs;

actor* kcreate_actor(size_t type);

extern actor* AK;

#endif
