#include "ko.h"


static msg* msg_view(ko* f) {
  return MSG(f);
}

const char* get_msg(msg* m) {
  return m->msg;
}

static void msg_cleanup(ko* o) {
  msg * m = (msg*)o;
  kfree(m->msg);
}

msg* mk_msg(const char* m) {
  msg* o = (msg*) mk_ko(sizeof(msg), msg_cleanup, msg_view, KO_MESSAGE);
  o->msg = kstrclone(m);
  return o;
}
