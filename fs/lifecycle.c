#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void nop(int i) {}

Directory* ctor_directory(Node** args) {
  Directory* d = malloc(sizeof(Directory));
  memset(d->entries, 0, sizeof(d->entries));

  for(int i = 0; i < ENTRIES_PER_DIR; i++) {
    if (!args[i]) break;

    assert(args[i]->type == ENTRY);

    d->entries[i] = args[i]->e;

    args[i]->type = EMPTY;
    DTOR(args[i]);
  }
  return d;
}

Entry* ctor_entry(Node* name, Node* n) {
  Entry* e = malloc(sizeof(Entry));
  strncpy(e->name, name->s, NAME_LEN);
  e->n = n;

  DTOR(name);

  return e;
}

void dtor_entry(Entry *e) {
  DTOR(e->n);
  free(e);
}

Link* ctor_link(Node * path) {
  Link *l = malloc(sizeof(Link));
  l->path = path->s;
  path->type = EMPTY;
  DTOR(path);

  return l;
}

Block* ctor_block(Node* bid) {
  Block *b = malloc(sizeof(Block));
  b->bid = bid->i;
  b->data = 0;

  DTOR(bid);
  return b;
}

void dtor_directory(Directory* dir) {
  for(size_t i = 0; i < ENTRIES_PER_DIR; i++) {
    DTOR(dir->entries[i]);
  }
  free(dir);
}

void dtor_link(Link * link) {
  free(link->path);
  free(link);
}

void dtor_block(Block* b) {
  if (b->data) {
    free(b->data);
  }
  free(b);
}

Node* ctor_node(void* d, int type) {
  Node * n = malloc(sizeof(Node));
  n->type = type;
  n->dir = d; // its a union anyway, we can just assign away
  return n;
}

void dtor_node(Node* node) {
  switch (node->type) {
    case EMPTY: // nothing to do here
      break;
    case DIRECTORY:
      DTOR(node->dir);
      break;
    case BLOCK:
      DTOR(node->block);
      break;
    case LINK:
      DTOR(node->link);
      break;
    case ENTRY:
      DTOR(node->e);
      break;
    case STRING:
      DTOR(node->s);
      break;
    case INTEGER:
      DTOR(node->i);
      break;
    default:
      assert(0);
  }
  free(node);
}
