#include "dir.h"

ko* mk_queue();

static ko* sched(dir * d) {
  const char* a[] = {"runnable", "pop^", 0};
  ko* p = LOOKUP(KO(d), a); 
  if (p) {
    const char* b[] = {"runnable", 0};
    ko* queued = LOOKUP(KO(d), b); 
    LINK(queued, p, "push@");
    return p;
  } else {
    return NULL;
  }
}

ko* mk_dir(void);
ko* mk_numdir(void);

ko* mk_sched() {
  ko* d = mk_dir();
  SAFE_ADD(d, mk_queue(), "runnable");

  SAFE_ADD(d, bind0((bound_func)sched, d), "sched^");
  return d;
}
