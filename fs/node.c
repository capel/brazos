#include "fs.h"
#include <assert.h>
#include <string.h>

#define RETYPE(o, func, args...) \
  do { \
  switch ((o)->type) { \
    case DIRECTORY: \
      return dir_ ## func ((Directory*)o, ## args); \
    case _FILE: \
      return file_ ## func ((File*)o, ##  args);\
    case LINK: \
      return link_ ## func ((Link*)o, ## args);\
    default:\
      assert(0); \
  } } while(0);

struct Node {
  enum { DIRECTORY, LINK, _FILE } type;
  union {
    Directory* dir;
    Link* link;
    File* file;
  };
};

Node * dir2Node(Directory* d) {
  Node * n = malloc(sizeof(Node));
  n->type = DIRECTORY;
  n->dir = d;
  return n;
}

Node * link2Node(Link* l) {
  Node * n = malloc(sizeof(Node));
  n->type = LINK;
  n->link = l;
  return n;
}

Node * file2Node(File* f) {
  Node * n = malloc(sizeof(Node));
  n->type = _FILE;
  n->file = f;
  return n;
}

void dtor_node(Node* n) {
  RETYPE(n, dtor);
}

int node_read(Node* n, size_t pos, void *buf, size_t nbytes) {
  RETYPE(n, read, pos, buf, nbytes);
}

int node_write(Node* n, size_t pos, const void *buf, size_t nbytes) {
  RETYPE(n, write, pos, buf, nbytes);
}

int node_sync(Node* n) {
  RETYPE(n, sync);
}

