#include "../variant.h"
#include "../vv.h"
#include "../stdlib.h"
#include "../kio.h"
#include "../syscalls.h"
#include "../sched.h"

#define HOPE(x,y) do { if (!x) { return y; } } while(0)

int sys_send(const char * s, size_t size) {
  if (!s) return E_BAD_ARG;

  V(v) = parse(s);
  HOPE(!IS_N(v), E_MALFORMED);
  V(e) = eval(v);
  HOPE(!IS_N(v), E_MALFORMED);

  push(cp()->q, e);

  return 0;
}

int sys_recv(char *buf, size_t size) {
  if (len(cp()->q) == 0) {
    return E_NO_DATA;
  } else {
    V(s) = serialize(pop(cp()->q));
    strncpy(buf, size, s.s); 
    return 0;
  }
}
