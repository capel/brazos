#include "ko.h"

future* mk_future() {
  future * f = kmalloc(sizeof(future));
  KO(f)->cleanup = future_cleanup;
  KO(f)->type = KO_FUTURE;
  KO(f)->rc = 1;
  f->data = 0;

  return f;
}
