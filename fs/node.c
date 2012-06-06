#include "fs.h"
#include <assert.h>
#include <string.h>

#define RETYPE(o, func, args...) \
  do { \
  switch ((o)->type) { \
    case DIRECTORY: \
      return dir_ ## func (o->dir, ## args); \
    case _FILE: \
      return file_ ## func (o->file, ##  args);\
    case LINK: \
      return link_ ## func (o->link, ## args);\
    default:\
      assert(0); \
  } } while(0);

struct Node {
  enum { DIRECTORY = 0, LINK = 1, _FILE =2 } type;
  union {
    Directory* dir;
    Link* link;
    File* file;
  };
};

int node_type(Node* n) {
  return n->type;
}


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

void node_dtor(Node* n) {
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

int node_size(Node* n) {
  RETYPE(n, size);
}

char* node_serialize(Node* n) {
  RETYPE(n, serialize);
}

bool is_dir(Node* n) { return n->type == DIRECTORY; }
bool is_link(Node* n) { return n->type == LINK; }
bool is_file(Node* n) { return n->type == _FILE; }

Directory* get_dir(Node *n) { assert(is_dir(n)); return n->dir; }
File* get_file(Node *n) { assert(is_file(n)); return n->file; }
Link* get_link(Node *n) { assert(is_link(n)); return n->link; }

