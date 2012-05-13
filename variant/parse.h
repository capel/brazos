#ifndef VARIANT_H
#define VARIANT_H

struct tuple;
struct map;

typedef struct variant {
  size_t type;
  union {
    int i;
    const char * s;
    struct map * s;
    struct tuple * s;
    int o;
  };
} variant;

variant parse(const char* s);
const char* s serialize(const variant v);

#endif
