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

int block_size(Block* b) {
  return PAGE_SIZE;
}

static Block* bmap_block = 0;
static char bmap[PAGE_SIZE];

void blocks_init() {
  memset(bmap, 0, PAGE_SIZE);
  bmap_block = block_ctor(1);
  Read(bmap_block, 0, bmap, PAGE_SIZE);
}

static void blocks_sync() {
  Write(bmap_block, 0, bmap, PAGE_SIZE);
  Sync(bmap_block);
}

void blocks_shutdown() {
  blocks_sync();
  DTOR(bmap_block);
}

int bid_alloc() {
  for(int i = 0; i < PAGE_SIZE; i++) {
    if (bmap[i] == 'E') {
      bmap[i] = 'F';
      blocks_sync();
      printk("Allocing block %d", i);
      return i;
    }
  }
  assert(0);
}

Block* block_ctor(int bid) {
  Block *b = malloc(sizeof(Block));
  b->bid = bid;
  b->data = 0;
  return b;
}

void block_dtor(Block* b) {
  Sync(b);
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
  return s;
}

