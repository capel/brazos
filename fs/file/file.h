#ifndef SPECIAL_H
#define SPECIAL_H

#include <string.h>
#include <vector.h>
#include "../fs.h"

struct File {
  struct special* s;
  union {
    struct {
      vector* v;
      size_t size;
    };
  };
};

typedef int (*write_func)(File* f, size_t pos, const void* buf, size_t nbytes);
typedef int (*read_func)(File* f, size_t pos, void* buf, size_t nbytes);
typedef int (*sync_func)(File* f);
typedef int (*size_func)(File* f);
typedef void (*dtor_func)(File* f);
typedef char* (*serialize_func)(File* f);

typedef struct special {
  write_func write;
  read_func read;
  sync_func sync;
  size_func size;
  dtor_func dtor;
  serialize_func serialize;
  size_t type;
} special;

File* raw_file_ctor();

#endif
