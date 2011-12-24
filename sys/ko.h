
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

typedef struct ko {
  cleanup_func cleanup;
  unsigned type;
  int rc;
} ko;

typedef struct file {
  ko o;
  struct file_vtable * v;
} file;

typedef struct dir {
  file f;
  struct dir_vtable * v;
} dir;

struct sinkhole;
typedef ko* (*sink_func)(void* data, ko* sunk);

typedef struct sinkhole {
  ko o;
  sink_func sink;
  void* data;
} sinkhole;


typedef ko* (*lookup_func)(dir* o, const char** path);
typedef err_t (*link_func)(dir* d, ko* child, const char* name);
typedef err_t (*unlink_func)(dir* d, const char* name);
typedef err_t (*map_func)(file* o, size_t* out_size, void** out_ptr);
typedef err_t (*unmap_func)(file* o, void* ptr);

typedef struct ko_vtable {
  cleanup_func cleanup;
} ko_vtable;

typedef struct file_vtable {
  map_func map;
  unmap_func unmap;
} file_vtable;;

typedef struct dir_vtable {
  lookup_func lookup;
  link_func link;
  unlink_func unlink;
} dir_vtable;

#define KO_UNKNOWN 0
#define KO_FILE 1
#define KO_DIR 2
#define KO_BOUND 4 // user will never see this
#define KO_SINKHOLE 8

#define IS_DIR(e) (KO(e)->type & KO_DIR)
#define IS_FILE(e) (KO(e)->type & KO_FILE || KO(e)->type & KO_DIR)
#define IS_BOUND(e) (KO(e)->type & KO_BOUND)
#define IS_SINKHOLE(e) (KO(e)->type & KO_SINKHOLE)

#define KO(e) ((ko*)e)
#define DIR(e) ((dir*)e)
#define FILE(e) ((file*)e)
#define SINKHOLE(e) ((sinkhole*)e)

#define PATH(v) ((const char**)(v)->data)

#define LOOKUP(e, path) ((e)->v->lookup((e), (path)))
#define LINK(e, child, name) ((e)->v->link((e), KO(child), (name)))
#define UNLINK(e, a, name) ((e)->v->unlink((e), (name)))

#define MAP(e, size, ptr) ((e)->v->map((e), (size), (ptr)))
#define UNMAP(e) ((e)->v->unmap((e)))

#define CLEANUP(e) (KO(e)->cleanup((e)))

#define SINK(e, s) ((e)->sink((e)->data, KO(s)))


typedef ko* (*bound_func)(void*);
ko* bind(bound_func func, void* data);
ko* release(ko* bound);
#define BIND(func, data) bind((bound_func)(func), data)

sinkhole* mk_sinkhole(sink_func, void* data);
#define MK_SINKHOLE(func, data) mk_sinkhole((sink_func)(func), data)

#define IGET_FUNC(name, type, prop) \
  static file* name(type * o) { \
    char n[32]; \
    itoa(n, 32, o->prop); \
    return mk_msg(n); \
  }

#define SAFE_ADD(parent, child, name) do { \
  ko* o = KO(child); \
  LINK(parent, o, name); \
  kput(o);\
} while(0) 

#define kput(e) \
do { \
  KO(e)->rc--; \
  if (KO(e)->rc == 0) { \
    printk("CLEANUP %k", e); \
    CLEANUP(KO(e)); \
    kfree(e); \
  } \
} while(0)

#define kget(e) KO(e)->rc++, e

file* mk_msg(const char* msg);
file* mk_file(void* ptr, size_t size);
dir* mk_dir(void);

#endif
