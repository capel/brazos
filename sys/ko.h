
#ifndef KO_H
#define KO_H

#include <types.h>
#include <stdlib.h>
#include "../kvector.h"
#include "../kio.h"
#include "../syscalls.h"

typedef size_t err_t;
struct ko;
struct msg;

typedef void (*cleanup_func)(struct ko* o);
typedef struct msg* (*view_func)(struct ko* o);

typedef struct ko {
  cleanup_func cleanup;
  view_func view;
  int rc;
  size_t flags;
  size_t type;
  int id;
} ko;

typedef struct msg {
  ko o;
  char* msg;
} msg;

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

typedef ko* (*dredge_func)(void* data);
typedef struct fountain {
  ko o;
  dredge_func dredge;
  void* data;
} fountain;

typedef struct future {
  ko o;
  ko *data;
  sinkhole* listener;
} future;

typedef ko* (*lookup_func)(dir* o, const char* path);
typedef ko* (*link_func)(dir* d, ko* child, const char* name);
typedef ko* (*unlink_func)(dir* d, const char* name);

typedef struct dir_vtable {
  lookup_func lookup;
  link_func link;
  unlink_func unlink;
} dir_vtable;

#define KO_WINDOW 1
#define IS_WINDOW(e) (FLAGGED((e), KO_WINDOW))
#define KO_RESOLVED 2
#define IS_RESOLVED(e) (FLAGGED(e, KO_RESOLVED))

#define KO_CTOR 4
#define IS_CTOR(e) (FLAGGED(e, KO_CTOR))

#define IS_DIR(e) (KO(e)->type == KO_DIR)
#define IS_MSG(e) (KO(e)->type == KO_MESSAGE)
#define IS_SINKHOLE(e) (KO(e)->type == KO_SINKHOLE)
#define IS_FUTURE(e) (KO(e)->type == KO_FUTURE)
#define IS_ERR(e) (KO(e)->type == KO_ERROR) 
#define IS_FOUNTAIN(e) (KO(e)->type == KO_FOUNTAIN)

#define MSG(e) ((msg*)e)
#define KO(e) ((ko*)e)
#define DIR(e) ((dir*)e)
#define SINKHOLE(e) ((sinkhole*)e)
#define FUTURE(e) ((future*)e)
#define FOUNTAIN(e) ((fountain*)e)

#define PATH(v) ((const char**)(v)->data)

#define LOOKUP(e, path) ((e)->v->lookup((e), (path)))
#define LINK(e, child, name) ((e)->v->link((e), KO(child), (name)))
#define UNLINK(e, name) ((e)->v->unlink((e), (name)))

#define VIEW(e) (KO(e)->view((e)))
#define CLEANUP(e) (KO(e)->cleanup((e)))

#define SINK(e, s) ((e)->sink((e)->data, KO(s)))
#define DREDGE(e) ((e)->dredge((e)->data))

#define SET_FLAG(e, f) (KO(e)->flags |= (f))
#define FLAGGED(e, f) (KO(e)->flags & (f))

void RESOLVE(future * f, ko* o);

#define GET_FUTURE(e) ((e)->data)

typedef msg* (*window_func)(void*);
ko* mk_window(window_func func, void* data);

sinkhole* mk_sinkhole(sink_func, void* data);

void _kput(ko* o, const char* file, const char* func);
#define kput(o) _kput(KO(o), __FILE__, __func__)

#define kget(e) KO(e)->rc++, e

#define ID(e) (KO(e)->id)

ko* mk_ko(size_t size, cleanup_func cleanup, view_func view, size_t type);
msg* mk_msg(const char* msg);
const char* get_msg(msg* o);

typedef ko* (*ctor_func)(void);
ko* mk_ctor(ctor_func func, int type);
ko* construct(ko* ctor);

dir* mk_dir();
future * mk_future(void);
void set_listener(future* f, sinkhole* sh);

struct work;
typedef void (*work_func)(void*);
struct work* mk_work(work_func func, void* data);
void do_work(struct work* w);

struct wq;
void push_work(struct wq* q, struct work* w);
struct work* pop_work(struct wq* q);

ko* walk(dir* start, const char* path);
dir* root();

void setup_ko_registry(void);
ko* get_ko(size_t id);
const char* ko_str(ko* o);

ko* mk_fountain(dredge_func func, void* data);

void setup_err_ko(void);

#define IGET_FUNC(name, type, prop) \
  static msg* name(void * o) { \
    type * t = (type*)o; \
    char n[32]; \
    itoa(n, 32, t->prop); \
    return mk_msg(n); \
  }

#define CS_VIEW_FUNC(name, s) \
static msg* name(ko * o) { \
  return mk_msg(s); \
}

#define SAFE_ADD(parent, child, name) do { \
  ko* o = KO(child); \
  LINK(parent, o, name); \
  kput(o);\
} while(0) 

#endif
