#include "dir.h"

static ko* sched(numdir * d) {
  unsigned key = kihm_lowest(d->h);
  ko* o = kihm_lookup(d->h, key);
  
  const char* a[] = {"queued", 0};
  ko* queued = LOOKUP(KO(d), a); 

  const char* n = kitoa(key);
  LINK(queued, o, n);
  kfree((void*)n);

  kihm_delete(d->h, key);
  
  return o;
}

ko* mk_dir(void);
ko* mk_numdir(void);


ko* mk_sched() {
  ko* d = mk_dir();
  SAFE_ADD(d, mk_numdir(), "procs");
  SAFE_ADD(d, mk_numdir(), "runnable");

  SAFE_ADD(d, mk_numdir(), "queued");

  SAFE_ADD(d, bind0((bound_func)sched, d), "sched!");
  return d;
}
