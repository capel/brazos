#include "../variant.h"
#include "../vv.h"
#include "../stdlib.h"
#include "../kio.h"
#include "../syscalls.h"
#include "../sched.h"

#define HOPE(x,y) do { if (!x) { dec(v); return y; } } while(0)

int sys_send(const char * s, size_t size) {
  if (!s) return E_BAD_ARG;

  variant v = parse(s);
  HOPE(IS_T(v), E_MALFORMED);
  HOPE(len(v) >= 2, E_MALFORMED);
  set(cp()->handles, idx(v,0), idx(v,1));

  dec(v);
  return 0;
}

int sys_recv(const char * id, char *buf, size_t size) {
  if (!id || !buf || size == 0) return E_BAD_ARG;

  variant v = parse(id);
  HOPE(!IS_N(v), E_MALFORMED);

  variant val = get(cp()->handles, v);
  variant s = serialize(val);

  if (len(s) > size) { 
    dec(s);
    dec(v);
    return E_NO_SPACE;
  }

  memcpy(buf, s.s, len(s));
  buf[len(s)] = '\0';
  dec(s);
  dec(v);

  return len(s);
}
