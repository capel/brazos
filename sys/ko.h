
#ifndef KO_H
#define KO_H

#include "../types.h"
#include "../kvector.h"
#include "../kio.h"
#include "../mem.h"
#include "../syscalls.h"

typedef size_t err_t;
struct ko;

typedef void (*cleanup_func)(struct ko* o);
typedef const char* (*view_func)(struct ko* o);

typedef struct ko {
  cleanup_func cleanup;
  view_func view;
  int rc;
  size_t flags;
  size_t type;
  size_t id;
} ko;

typedef struct dir {
  ko f;
  struct dir_vtable * v;
} dir;

struct sinkhole;
typedef ko* (*sink_func)(void* data, ko* sunk);

typedef struct sinkhole {
  ko o;
  sink_func sink;
  void* data;
} sinkhole;

typedef struct future {
  ko o;
  ko *data;
} future;

typedef ko* (*lookup_func)(dir* o, const char* path);
typedef err_t (*link_func)(dir* d, ko* child, const char* name);
typedef err_t (*unlink_func)(dir* d, const char* name);

typedef struct dir_vtable {
  lookup_func lookup;
  link_func link;
  unlink_func unlink;
} dir_vtable;

#define KO_BOUND 1
#define IS_BOUND(e) (FLAGGED((e), KO_BOUND))
#define KO_RESOLVED 2
#define IS_RESOLVED(e) (FLAGGED(e, KO_RESOLVED))

#define IS_DIR(e) (KO(e)->type == KO_DIR)
#define IS_MSG(e) (KO(e)->type == KO_MESSAGE)
#define IS_SINKHOLE(e) (KO(e)->type == KO_SINKHOLE)
#define IS_FUTURE(e) (KO(e)->type == KO_FUTURE)

#define KO(e) ((ko*)e)
#define DIR(e) ((dir*)e)
#define SINKHOLE(e) ((sinkhole*)e)
#define FUTURE(e) ((future*)e)

#define PATH(v) ((const char**)(v)->data)

#define LOOKUP(e, path) ((e)->v->lookup((e), (path)))
#define LINK(e, child, name) ((e)->v->link((e), KO(child), (name)))
#define UNLINK(e, name) ((e)->v->unlink((e), (name)))

#define VIEW(e) (KO(e)->view((e)))
#define CLEANUP(e) (KO(e)->cleanup((e)))

#define SINK(e, s) ((e)->sink((e)->data, KO(s)))

#define SET_FLAG(e, f) (KO(e)->flags |= (f))
#define FLAGGED(e, f) (KO(e)->flags & (f))

void RESOLVE(future * f, ko* o);

#define GET_FUTURE(e) ((e)->data)

typedef ko* (*bound_func)(void*);
ko* bind(bound_func func, int type, void* data);
ko* release(ko* bound);
#define BIND_MSG(func, data) bind((bound_func)(func), KO_MESSAGE, data)
#define BIND(func, type, data) bind((bound_func)(func), type, data)

sinkhole* mk_sinkhole(sink_func, void* data);
#define MK_SINKHOLE(func, data) mk_sinkhole((sink_func)(func), data)

#define IGET_FUNC(name, type, prop) \
  static ko* name(type * o) { \
    char n[32]; \
    itoa(n, 32, o->prop); \
    return mk_msg(n); \
  }

#define SAFE_ADD(parent, child, name) do { \
  ko* o = KO(child); \
  LINK(parent, o, name); \
  kput(o);\
} while(0) 

void _kput(ko* o, const char* file, const char* func);
#define kput(o) _kput(KO(o), __FILE__, __func__)

#define kget(e) KO(e)->rc++, e

#define ID(e) (KO(e)->id)

ko* mk_ko(size_t size, cleanup_func cleanup, view_func view, size_t type);
ko* mk_msg(const char* msg);
dir* mk_dir();
future * mk_future(void);

ko* walk(dir* start, const char* path);

void setup_ko_registry(void);
ko* get_ko(size_t id);
const char* ko_str(ko* o);


#endif
