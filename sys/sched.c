#include "ko.h"

/*
ko* mk_queue();

static ko* sched(dir * d) {
  const char* a[] = {"runnable", "pop^", 0};
  ko* p = LOOKUP(d, a); 
  if (p) {
    const char* b[] = {"runnable", 0};
    ko* queued = LOOKUP(d, b); 
    assert(IS_DIR(queued));
    LINK(DIR(queued), p, "push@");
    return p;
  } else {
    return NULL;
  }
}

dir* mk_sched() {
  dir* d = mk_dir();
  SAFE_ADD(d, mk_queue(), "runnable");

  SAFE_ADD(d, BIND(sched, d), "sched^");
  return d;
}
*/
