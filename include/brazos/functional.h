#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#define fori(_x, n) for(size_t _x = 0; _x < n; _x++)

#define each(type, x, v, expr) \
 ({ fori(i, vsize(v)) {\
   type x = (type) vget(v, i); \
   { expr; } \
  } })\

#define push(v, val) vpush(v, (void*)(val))

#define max(type, v) \
  ({  type res = (type) vget(v, 0); \
   each(type, x, v, res = (x > res) ? x : res); res; })

#define min(type, v) \
  ({  type res = (type) vget(v, 0); \
   each(type, x, v, res = (x < res) ? x : res); res; })

#define sum(type, v) \
  ({  type res = 0; each(type, x, v, res += x); res; })

#define NOT_FOUND ((size_t)-1)
#define findi(type, x, v, constraint) \
  ({  size_t res = NOT_FOUND; \
   fori(i, vsize(v)) { \
    type x = (type) vget(v, i); \
    if (constraint) { res = i; break; } \
   } \
  res; })

#define find(type, x, v, constraint) (type)vget(v, findi(type, x, v, constraint))
#define take(type, x, v, constraint) (type)vremove(v, findi(type, x, v, constraint))

#define reduce(type, x, v, old, expr) \
  ({  type old; \
   fori(idx, vsize(v)) { type x = (type) vget(v, idx); \
   if (idx == 0) { old = x; } else { old = expr; } } \
  old; })
  
#define filter(type, x, v, constraint) \
  ({  vector *n = make_vector(vsize(v)); \
  each(type, x,v, if (constraint) { push(n, x); }); \
  n; })
  
#define map(type, x, v, expr) \
  ({  vector *n = make_vector(vsize(v)); \
  each(type, x, v, push(n, (expr))); \
  n; })

#define all(type, x, v, constraint) \
  ({ bool res = true; \
  each(type, x, v, res = (constraint) ? res : false); \
  res; })

#define any(type, x, v, constraint) \
  ({ bool res = false; \
  each(type, x, v, res = (constraint) ? true : res); \
  res; })

#define contains(type, v, val) (NOT_FOUND != findi(type, x, v, x == val))
#define strcontains(type, v, val) (NOT_FOUND != findi(const char*, x, v, !strcmp(x, val)))

#endif
