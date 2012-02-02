#include "variant.h"

#include "stdlib.h"
#include "vv.h"

void tuple_cleanup(variant t);
variant tuple_serialize(variant t);

void map_cleanup(variant m);
variant map_serialize(variant m);

static unsigned str_hash(const char* s) {
  unsigned h = 0;
  unsigned m = 1;
  for(int i = 0; s[i]; i++) {
    h += m++ * (unsigned char)s[i];
  }
  return h;
}

static unsigned tuple_hash(variant t) {
  unsigned r = 0;
  foreach(x, t) {
    r ^= hash(x);
  }
  return r;
}


unsigned hash(variant v) {
  switch (v.type) {
    case V_I:
      return (unsigned)v.i;
    case V_S:
      return str_hash(v.s);
    case V_H:
      return (unsigned)v.h;
    case V_M:
      return (unsigned)v.t;
    case V_T:
    case V_C:
      return tuple_hash(v);
    case V_N:
      return 0;
    default:
      assert(0);
  }
  return false;
}

bool tuple_eq(variant a, variant b);

bool eq(variant a, variant b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case V_I:
      return a.i = b.i;
    case V_S:
      return a.len == b.len && strcmp(a.s, b.s);
    case V_H:
      return a.h == b.h;
    case V_M:
      return a.t == b.t;
    case V_T:
    case V_C:
      return tuple_eq(a, b);
    case V_N:
      return true;
    default:
      assert(0);
  }
  return false;
}

void _dec(variant v) {
  assert(v.rc && *v.rc == 0);
  free(v.rc);
  switch (v.type) {
    case V_S:
      free((void*)v.s);
      break;
    case V_M:
      map_cleanup(v);
      break;
    case V_T:
      tuple_cleanup(v);
      break;
    default:
      break;
  }
}

variant serialize(variant v) {
  char buf[48];
  char * s;
  switch (v.type) {
    case V_S:
      s = malloc(len(v) + 3);
      snprintf(s, len(v) + 3, "\"%s\"", v.s);
      return RStr(s);
    case V_M:
      return map_serialize(v);
    case V_T:
      return tuple_serialize(v);
    case V_I:
      itoa(buf, 48, v.i);
      return Str(buf);
    case V_H:
      snprintf(buf, 48, "<%c %d>", v.subtype, v.h);
      return Str(buf);
    case V_N:
      return Str("null");
    default:
      assert(0);
  }
}

variant Variant(vtype type) {
  variant v;
  v.type = type;
  if (type == V_I || type == V_H || type == V_N) {
    v.rc = 0;
  } else {
    v.rc = malloc(sizeof(size_t));
    *v.rc = 1;
  }
  return v;
}

size_t len(variant v) {
  switch (v.type) {
    case V_S:
    case V_T:
      return v.len;
    case V_I:
    case V_H:
      return 1;
    case V_M:
    case V_N:
      return 0;
    default:
      assert(0);
  }
}

variant idx(variant v, size_t pos) {
  char buf[2];
  switch (v.type) {
    case V_S:
      if (pos >= len(v)) return Null();
      buf[0] = v.s[pos];
      buf[1] = '\0';
      return Str(buf);
    case V_T:
      if (pos >= len(v)) return Null();
      return v.t[pos];
    case V_M:
      return Null();
    case V_I:
    case V_H:
      if (pos != 0) return Null();
      return v;
    case V_N:
      return v;
    default:
      assert(0);
  }
}

variant sslice(variant v, size_t s) {
  return slice(v, s, len(v));
}
variant eslice(variant v, size_t e) {
  return slice(v, 0, e);
}

variant slice(variant v, size_t s, size_t e) {
  char * buf;
  vv* t;

  if (s == e) return idx(v, s);
  if (s > e || e >= len(v)) { return Null(); }
  switch (v.type) {
    case V_S:
      buf = malloc(e - s + 1);
      memcpy(buf, v.s + s, e - s);
      buf[e - s + 1] = '\0';
      return RStr(buf);
    case V_T:
      t = Vv(s - e);
      for(size_t i = s; i <= e; i++) {
        push(t, idx(v, i));
      }
      variant r = Tvv(t);
      vvdec(t);
      return r;
    case V_M:
      return Null();
    case V_I:
    case V_H:
      return Null();
    case V_N:
      return v;
    default:
      assert(0);
  }
}


variant RStr(const char * s) {
  variant v = Variant(V_S);
  v.len = strlen(s);
  v.s = s;
  return v;
}

variant CStr(const char * s) {
  // ints wont clean anything up... so...
  variant v = Variant(V_I);
  v.type = V_S;
  v.len = strlen(s);
  v.s = s;
  return v;
}

variant Int(int i) {
  variant v = Variant(V_I);
  v.i = i;
  return v;
}

variant Str(const char *s) {
  variant v = Variant(V_S);
  v.len = strlen(s);
  v.s = strclone(s);
  return v;
}
  
