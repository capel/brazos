#include "ko.h"

static const char* view_future(ko* o) {
  return "IMPOSSIBLE";
}

static void cleanup(ko* o) {}

future* mk_future() {
  future * f = (future*) mk_ko(sizeof(future), cleanup, view_future, KO_FUTURE);
  f->data = 0;

  return f;
}

void RESOLVE(future* f, ko* o) {
  f->data = o;
  SET_FLAG(f, KO_RESOLVED);
}
