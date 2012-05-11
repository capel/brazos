#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Directory* ctor_directory() {
  Directory* d = malloc(sizeof(Directory));
  memset(d->nodes, 0, sizeof(d->nodes));
  return d;
}

Link* ctor_link(char* path) {
  Link *l = malloc(sizeof(Link));
  memset(l->path, 0, sizeof(l->path));
  strcpy(l->path, path);
  l->resolved = 0;
  return l;
}

Block* ctor_block(size_t bid) {
  Block *b = malloc(sizeof(Block));
  b->bid = bid;
  b->data = 0;
  return b;
}

void dtor_directory(Directory* dir) {
  for(size_t i = 0; i < NODES_PER_DIR; i++) {
    if (dir->nodes[i]) {
      dtor_node(dir->nodes[i]);
    }
  }
  free(dir);
}

void dtor_link(Link * link) {
  if (link->resolved) {
    dtor_node(link->resolved);
  }
  free(link);
}

void dtor_block(Block* b) {
  if (b->data) {
    free(b->data);
  }
  free(b);
}

Node* ctor_node(char* name) {
  Node* n = malloc(sizeof(Node));
  strncpy(n->name, name, NAME_LEN);
  n->type = INVALID;
  return n;
}

Node* dir2Node(char* name, Directory* dir) {
  Node * n = ctor_node(name);
  n->type = DIRECTORY;
  n->dir = dir;
  return n;
}

Node* block2Node(char* name, Block* block) {
  Node * n = ctor_node(name);
  n->type = BLOCK;
  n->block = block;
  return n;
}

Node* link2Node(char* name, Link* link) {
  Node * n = ctor_node(name);
  n->type = LINK;
  n->link = link;
  return n;
}

void dtor_node(Node* node) {
  switch (node->type) {
    case DIRECTORY:
      dtor_directory(node->dir);
      return;
    case BLOCK:
      dtor_block(node->block);
      return;
    case LINK:
      dtor_link(node->link);
      return;
    default:
      assert(0);
  }
}
