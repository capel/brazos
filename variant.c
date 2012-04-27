#include "variant.h"
#include "stdlib.h"

void tuple_cleanup(variant t);
variant tuple_serialize(variant t);

void map_cleanup(variant m);
variant map_serialize(variant m);

void func_cleanup(variant t);
variant func_serialize(variant t);

void call_cleanup(variant t);
variant call_serialize(variant t);
variant call_eval(variant t);

static unsigned str_hash(const char* s) {
  unsigned h = 0;
  unsigned m = 1;
  for(int i = 0; s[i]; i++) {
    h += m++ * (unsigned char)s[i];
  }
  return h;
}

static unsigned list_hash(variant t) {
  unsigned r = 0;
  int i = 1;
  foreach(x, t) {
    r += i++ * hash(x);
  }
  return r;
}


unsigned hash(variant v) {
  switch (v.type) {
    case V_I:
      return (unsigned)v.i;
    case V_S:
      return str_hash(v.s);
    case V_M:
      return (unsigned)v.t;
    case V_L:
      return list_hash(v);
    case V_F:
      return (unsigned)v.f;
    case V_N:
      return 0;
    default:
      assert(0);
  }
  return false;
}

bool list_eq(variant a, variant b);

bool eq(variant a, variant b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case V_I:
      return a.i = b.i;
    case V_S:
      return a.len == b.len && !strcmp(a.s, b.s);
    case V_M:
      return a.t == b.t;
    case V_L:
      return list_eq(a,b);
    case V_F:
      return a.f == b.f;
    case V_N:
      return true;
    default:
      assert(0);
  }
  return false;
}

void list_cleanup(variant v);
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
    case V_L:
      list_cleanup(v);
      break;
    case V_F:
      func_cleanup(v);
      break;
    default:
      break;
  }
}

variant list_serialize(variant v);
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
    case V_L:
      return list_serialize(v);
    case V_F:
      return func_serialize(v);
    case V_I:
      itoa(buf, 48, v.i);
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
  if (type == V_I || type == V_N) {
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
      return v.len;
    case V_L:
      return v.l->len;
    case V_I:
    case V_F:
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
    case V_L:
      if (pos >= len(v)) return Null();
      return v.l->t[pos];
    case V_M:
      return Null();
    case V_I:
    case V_F:
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
  variant t;

  if (s == e) return Null();
  if (s > e || e > len(v)) return Null();
  switch (v.type) {
    case V_S:
      buf = malloc(e - s + 1);
      memcpy(buf, v.s + s, e - s);
      buf[e - s + 1] = '\0';
      return RStr(buf);
    case V_L:
      t = List(e- s);
      for(size_t i = s; i < e; i++) {
        push(t, idx(v, i));
      }
      return t;
    case V_M:
      return Null();
    case V_I:
    case V_F:
      return Null();
    case V_N:
      return v;
    default:
      assert(0);
  }
}

variant list_eval(variant v);

variant eval(variant v) {
  if (v.type == V_L) return list_eval(v);
  return v;
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
  
void pdec(variant *v ) { 
  dec(*v);
}
