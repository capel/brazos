#include <mach/io.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <extras.h>
#include "fs.h"


#include <time.h>

struct Block {
  size_t bid;
  char* data;
};

int bid_alloc() {
  return (int) time(NULL);
}

Block* block_ctor(int bid) {
  Block *b = malloc(sizeof(Block));
  b->bid = bid;
  b->data = 0;
  return b;
}

void block_dtor(Block* b) {
  if (b->data) {
    free(b->data);
  }
  free(b);
}

static void init_block(Block* b) {
  if (b->data == NULL) {
    b->data = calloc(PAGE_SIZE, 1);
    bload(b->bid, b->data);
  }
}

int block_read(Block* b, size_t pos, void *buf, size_t nbytes) {
  assert(buf);
  if (!nbytes) return 0;

  init_block(b);

  if (pos + nbytes > PAGE_SIZE) {
    nbytes = PAGE_SIZE - pos;
  }

  memcpy(buf, (char*)b->data + pos, nbytes);
  return nbytes;
}

int block_write(Block* b, size_t pos, const void *buf, size_t nbytes) {
  assert(buf);
  if (!nbytes) return 0;

  init_block(b);

  if (pos + nbytes > PAGE_SIZE) {
    nbytes = PAGE_SIZE - pos;
  }

  memcpy((char*)b->data + pos, buf, nbytes);
  return nbytes;
}

int block_sync(Block* b) {
  if (!b->data) return 0;

  bsync(b->bid, b->data);
  free(b->data);
  b->data = 0;
  return 0;
}

char* block_serialize(Block * b) {
  int needed = strlen("B()" + 33);
  char *s = malloc(needed);
  snprintf(s, needed, "B(%zd)", b->bid);
  printk("%s", s);
  return s;
}

