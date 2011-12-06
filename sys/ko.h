
#ifndef KO_H
#define KO_H

#include "../types.h"
#include "../kfs.h"

typedef struct _ko {
  struct _vtable *v;
  unsigned type;
  int rc;
} ko;

typedef size_t err_t;

typedef ko* (*lookup_func)(ko* o, const char** path);
typedef err_t (*link_func)(ko* o, ko* child, const char* name);
typedef err_t (*unlink_func)(ko* o, const char* name);
typedef err_t (*map_func)(ko* o, size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(ko* o, void* ptr);
typedef void (*cleanup_func)(ko* o);

typedef struct _vtable {
  lookup_func lookup;
  link_func link;
  unlink_func unlink;
  map_func map;
  unmap_func unmap;
  cleanup_func cleanup;
} vtable;

#define KO(e) ((ko*)e)

#define LOOKUP(e, path) (KO(e)->v->lookup((e), (path)))
#define LINK(e, child, name) (KO(e)->v->link((e), (child), (name)))
#define UNLINK(e, a, name) (KO(e)->v->unlink((e), (name)))
#define MAP(e, o, ptr) (KO(e)->v->map((e), (o), (ptr)))
#define UNMAP(e) (KO(e)->v->unmap((e)))
#define CLEANUP(e) (KO(e)->v->cleanup((e)))


#define KO_BOUND 4
#define KO_OBJ 1
typedef ko* (*bound_func)(void*);
ko* bind0(bound_func func, void* data);
ko* release0(ko* bound);
#define IS_BOUND(e) (KO(e)->type == KO_BOUND)

#define IGET_FUNC(name, type, prop) \
  static ko* name(type * o) {return mk_msg(kitoa(o->prop)); }

#define SAFE_ADD(parent, child, name) do { \
  ko* o = (child); \
  LINK(parent, o, name); \
  kput(o);\
} while(0) 

#define kput(e) KO(e)->rc--
#define kget(e) KO(e)->rc++

err_t void_unmap(ko* o, void* ptr);
ko* no_lookup(ko* o, char** path);
err_t no_link(ko* o, ko* child, const char* name);
err_t no_unlink(ko* o, void* ptr);

ko* mk_msg(const char* msg);

#endif
