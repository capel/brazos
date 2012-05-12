#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static char* serialize_directory(Directory* dir) {
  size_t needed = strlen("D()") + 1;
  char* node_strs[NODES_PER_DIR];
  memset(node_strs, 0, sizeof(node_strs));

  for(size_t i = 0; i < NODES_PER_DIR; i++) {
    node_strs[i] = dir->nodes[i] ? serialize(dir->nodes[i]) : NULL;
    needed += dir->nodes[i] ? strlen(node_strs[i]) + 1 : 0; // 1 for comma
  }

  char* s = malloc(needed);
  char* orig = s;
  sprintf(s, "D(");
  s += strlen("D(");

  for(size_t i = 0; i < NODES_PER_DIR; i++) {
    if (node_strs[i]) {
      sprintf(s, "%s,", node_strs[i]);
      s += strlen(node_strs[i]) + 1;
      free(node_strs[i]);
    }
  }

  sprintf(s, ")");

  return orig;
}

static char* serialize_block(Block * b) {
  char *s = malloc(strlen("B()" + 32));
  sprintf(s, "B(%zd)", b->bid);
  return s;
}

static char* serialize_link(Link * l) {
  char *s = malloc(strlen("L()" + strlen(l->path) + 1));
  sprintf(s, "L(%s)", l->path);
  return s;
}


static void indent_print(int indent) {
  for(int i = 0; i < indent; i++) {
    printf(" ");
  }
}

void pretty_print(Node* n, int indent) {
  indent_print(indent);
  switch(n->type) {
    case DIRECTORY:
      printf("%s :\n", n->name);
      for (size_t i = 0; i < NODES_PER_DIR; i++) {
        if (n->dir->nodes[i]) {
          pretty_print(n->dir->nodes[i], indent + 2);
        }
      }
      return;
    case LINK:
      printf("%s : %s@\n", n->name, n->link->path);
      return;
    case BLOCK:
      printf("%s : [%zd]\n", n->name, n->block->bid);
      return;
    default:
      assert(0);
  }
}

char* serialize(Node* node) {
  char * tmp;
  switch (node->type) {
    case DIRECTORY:
      tmp = serialize_directory(node->dir);
      break;
    case BLOCK:
      tmp = serialize_block(node->block);
      break;
    case LINK:
      tmp = serialize_link(node->link);
      break;
    default:
      assert(0);
  }
  char * s = malloc(1 + strlen(":") + strlen(node->name) + strlen(tmp));
  sprintf(s, "%s:%s", node->name, tmp);
  free(tmp);

  return s;
}
