#define ACTOR_KERNEL 1
#define ACTOR_USER 2

typedef struct {
    size_t rc;
    void* aid;
    void* d2;
    struct _actor_funcs* f;
} actor;

typedef err_t (*add_mapping_func)(actor* a, const void* ptr, 
               const size_t size, const perms_t perms);
typedef err_t (*remove_mapping_func)(actor* a, struct _ent* e);

typedef struct _actor_funcs {
    add_mapping_func add_mapping;
    remove_mapping_func remove_mapping;
} actor_funcs;

