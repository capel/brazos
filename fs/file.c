#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <extras.h>

struct File {
  vector* v;
  size_t size;
};

int file_size(File* f) {
  return f->size;
}

File* file_ctor(int size, Block** b, int num_blocks) {
  File * f = malloc(sizeof(File));

  // empty file
  if (num_blocks == 0) {
    assert(size == 0);
    f->v = make_vector(1);
    push(f->v, block_ctor(bid_alloc()));
    f->size = 0;
  } else {
    f->v = make_vector(num_blocks);
    for(int i = 0; i < num_blocks; i++) {
      push(f->v, b[i]);
    }
    f->size = size;
  }
  return f;
}

void file_dtor(File* f) {
  each(Block*, b, f->v, DTOR(b));
  cleanup_vector(f->v);
  free(f);
}

int file_read(File* f, size_t pos, void *buf, size_t nbytes) {
  return Read((Block*)vget(f->v, 0), pos, buf, nbytes);
}

int file_write(File* f, size_t pos, const void *buf, size_t nbytes) {
  return Write((Block*)vget(f->v, 0), pos, buf, nbytes);
}

int file_sync(File* f) {
  each(Block*, b, f->v, Sync(b));
  return 0;
}

char* file_serialize(File* f) {
  size_t needed = strlen("F()") + 33;

  vector* v = map(Block*, b, f->v, block_serialize(b));

  const char* inner = vector_join(v, " ");
  needed += strlen(inner) + 1;

  char* s = malloc(needed);

  snprintf(s, needed, "F(%zd %s)", f->size, inner);
  free((char*)inner);
  each(char*, s, v, free(s));

  cleanup_vector(v);
  printk("file %s", s);
  return s;
}
