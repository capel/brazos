
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
  unsigned type;
  int rc;
  size_t id;
} ko;

/*
typedef struct file {
  ko o;
  struct file_vtable * v;
} file;
*/

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

/*
typedef err_t (*map_func)(file* o, size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(file* o, void* ptr);
*/

/*
typedef struct file_vtable {
  map_func map;
  unmap_func unmap;
} file_vtable;
*/

typedef struct dir_vtable {
  lookup_func lookup;
  link_func link;
  unlink_func unlink;
} dir_vtable;


#define IS_DIR(e) (KO(e)->type == KO_DIR)
#define IS_MSG(e) (KO(e)->type == KO_MESSAGE)
#define IS_BOUND(e) (KO(e)->type == KO_BOUND)
#define IS_SINKHOLE(e) (KO(e)->type == KO_SINKHOLE)
#define IS_FUTURE(e) (KO(e)->type == KO_FUTURE)
#define IS_RESOLVED(e) (KO(e)->type == KO_RESOLVED)

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

#define RESOLVE(e, o) do { \
  ((e)->data = (o)); \
  KO(e)->type = KO_RESOLVED; \
} while (0);

#define GET_FUTURE(e) ((e)->data)

typedef ko* (*bound_func)(void*);
ko* bind(bound_func func, void* data);
ko* release(ko* bound);
#define BIND(func, data) bind((bound_func)(func), data)

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
