#include "ko.h"
#include "../stdlib.h"
#include "../mem.h"

typedef struct {
  ko o;
  const char* msg;
} msg;

static err_t msg_map(msg* msg, size_t* out_size, void** out_ptr) {
  *out_size = strlen(msg->msg);
  *out_ptr = msg;
  return 0;
}

static err_t msg_unmap(msg* m, void* ptr) {
  return 0;
}

static void msg_cleanup(msg* m) {
  kfree((void*)m->msg);
  kfree(m);
}

vtable msg_vt = {
  .lookup = (lookup_func)no_lookup,
  .link = (link_func)no_link,
  .unlink = (unlink_func)no_unlink,
  .map = (map_func)msg_map,
  .unmap = (unmap_func)msg_unmap,
  .cleanup = (cleanup_func)msg_cleanup,
};

ko* mk_msg(const char* m) {
  msg* d = kmalloc(sizeof(msg));
  d->o.type = KO_OBJ;
  d->o.v = &msg_vt;
  d->o.rc = 1;

  d->msg = m;
  return (ko*)d;
}



