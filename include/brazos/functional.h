#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#define foreach(type, x, idx, v) \
  size_t idx = 0; \
  for(type x = (type)v->data[0]; idx < v->size; x = (type)v->data[++idx])

#define push(v, val) vector_push(v, (void*)(val))

#define max(type, v) \
  ({  type res = (type) v->data[0]; \
  foreach(type, x, idx, v) { if (x > res) { res = x; }  } \
  res; })

#define min(type, v) \
  ({  type res = (type) v->data[0]; \
  foreach(type, x, idx, v) { if (x < res) { res = x; }  } \
  res; })

#define sum(type, v) \
  ({  type res = 0; \
  foreach(type, x, idx, v) {  res +=  x; }  \
  res; })


#define find(type, x, v, constraint) \
  ({  type res = 0; \
  foreach(type, x, idx, v) { if (constraint) { res = x; break; }  } \
  res; })

#define take(type, x, v, constraint) \
  ({  type res = 0; \
  foreach(type, x, idx, v) { if (constraint) { res = x; vector_remove(v, idx); break; }  } \
  res; })

#define reduce(type, x, v, old, expr) \
  ({  type old; \
  foreach(type, x, idx, v) { if (idx == 0) { old = x; } else { old = expr; } } \
  old; })
  
#define filter(type, x, v, constraint) \
  ({  vector *n = make_vector(v->size); \
  foreach(type, x, idx, v) { if (constraint) { push(n, x); }  } \
  n; })
  
#define map(type, x, v, expr) \
  ({  vector *n = make_vector(v->size); \
  foreach(type, x, idx, v) { push(n, (expr)); }  \
  n; })

#define each(type, x, v, expr) \
 ({  foreach(type, x, idx, v) { expr; } })



#define all(type, x, v, constraint) \
  ({ bool res = true; \
  foreach(type, x, idx, v) { if (constraint) { res = false; break; }  } \
  res; })

#define any(type, x, v, constraint) \
  ({ bool res = false; \
  foreach(type, x, idx, v) { if (constraint) { res = true; break; }  } \
  res; })

#define contains(type, v, val) \
  ({ bool res = false; \
  foreach(type, x, idx, v) { if (x == val) { res = true; break; }  } \
  res; })

#define strcontains(v, val) \
  ({ bool res = false; \
  foreach(char*, x, idx, v) { if (!strcmp(x, val)) { res = true; break; }  } \
  res; })


#endif
