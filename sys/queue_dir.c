#include "ko.h"


#include "../mem.h"
#include "../stdlib.h"
#include "../syscalls.h"


typedef struct {
  ko o;
  vector* v;
} queue;

static ko* q_lookup(queue* q, const char** path) {
  assert(path);
  if (!*path) {
    return (ko*)q;
  }
  
  if (!strcmp(*path, "push@")) {
    return mk_msg("Perhaps you shouldn't look into a sinkhole?");
  }
  if (!strcmp(*path, "size")) {
    char buf[32];
    utoa(buf, 32, q->v->size);
    mk_msg(buf);
  }

  if (strcmp(*path, "pop^")) {
    return NULL;
  }

  // path == pop
  if (q->v->size == 0) return NULL;

  ko* child = (ko*)q->v->data[0]; 
  vector_remove(q->v, 0);
  kput(child);

  if (!child) {
    return NULL;
  }
  
  if (IS_BOUND(child)) {
    child = release0(child);
  }

  return LOOKUP(child, path+1);
}

static err_t q_unlink(ko* d, const char* name) {
  return E_NOT_SUPPORTED;
}

static void q_cleanup(queue* d) {
  cleanup_vector(d->v);
  kfree(d);
}

static err_t q_map(ko* d, size_t* out_size, void** out_ptr) {
  *out_ptr = "push@/pop^/size";
  *out_size = strlen((char*)out_ptr);
  return 0;
}

static err_t q_unmap(ko* d, void* ptr) {
  return 0;
}

static err_t q_link(queue* q, ko* child, const char* name) {
  if (!name || strcmp(name, "push@")) {
    return E_NOT_SUPPORTED;
  }
  vector_push(q->v, (void*)child);
  kget(child);
  return 0;
}

static vtable q_vt = {
  .lookup = (lookup_func)q_lookup,
  .link = (link_func)q_link,
  .unlink = (unlink_func)q_unlink,
  .map = (map_func)q_map,
  .unmap = (unmap_func)q_unmap,
  .cleanup = (cleanup_func)q_cleanup,
};


ko* mk_queue() {
  queue * q = kmalloc(sizeof(queue));
  q->o.type = KO_OBJ;
  q->o.v = &q_vt;
  q->o.rc = 1;

  q->v = kmake_vector(sizeof(ko*), UNMANAGED_POINTERS);
  return (ko*)q;
}
