#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Node* dir2Node(char* name, Directory* dir);
Node* block2Node(char* name, Block* block);
Node* link2Node(char* name, Link* link);

int dir_slots_free(Directory* dir) {
  size_t n = 0;
  for(int i = 0; i < NODES_PER_DIR; i++) {
    if (dir->nodes[i] == NULL) {
      n++;
    }
  }
  return n;
}

Node* lookup(Directory* dir, char* name) {
  for(int i = 0; i < NODES_PER_DIR; i++) {
    if (dir->nodes[i] && !strcmp(dir->nodes[i]->name, name)) {
      return dir->nodes[i];
    }
  }
  return 0;
}

int link(Directory* dir, Node* n) {
  if (dir_slots_free(dir) == 0) {
    return E_FULL;
  }

  if (lookup(dir, n->name)) {
    return E_EXISTS;
  }

  for(int i = 0; i < NODES_PER_DIR; i++) {
    if (dir->nodes[i] == 0) {
      dir->nodes[i] = n;
      return SUCCESS;
    }
  }

  assert(0);
}

int unlink(Directory* dir, char* name) {
  if (!lookup(dir, name)) {
    return E_NOTFOUND;
  }

  for(int i = 0; i < NODES_PER_DIR; i++) {
    if (dir->nodes[i] && !strcmp(dir->nodes[i]->name, name)) {
      dtor_node(dir->nodes[i]);
      dir->nodes[i] = 0;
      return SUCCESS;
    }
  }

  assert(0);
}
