#include "ko.h"

typedef struct {
  ko o;
  char* msg;
} msg;

static const char* msg_view(ko* f) {
  msg *m = (msg*)f;
  printk("mapping msg %k with data %s", m, m->msg);
  return m->msg;
}

static void msg_cleanup(ko* o) {
  msg * m = (msg*)o;
  kfree(m->msg);
}

ko* mk_msg(const char* m) {
  msg* o = (msg*) mk_ko(sizeof(msg), msg_cleanup, msg_view, KO_MESSAGE);
  o->msg = kstrclone(m);
  return KO(o);
}
