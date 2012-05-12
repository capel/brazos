#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static Directory* parse_directory(char *s, size_t *pos) {
  assert(s[*pos] == 'D');
  *pos += 2;

  Directory* dir = ctor_directory();

  for(size_t i = 0; i < NODES_PER_DIR && s[*pos] != ')'; i++) {
    dir->nodes[i] = parse(s, pos);
    assert(s[*pos] == ',');
    *pos += 1;
  }

  *pos += 1; // eat )
  return dir;
}

static Block* parse_block(char *s, size_t *pos) {
  assert(s[*pos] == 'B');
  *pos += 2;

  char bid[32];
  memset(bid, 0, 32);
  char *buf = bid;

  while(s[*pos] != ')') {
    *buf++ = s[*pos];
    *pos += 1;
  }
  *pos += 1;

  return ctor_block(atoi(bid));
}


static Link* parse_link(char *s, size_t *pos) {
  assert(s[*pos] == 'L');
  *pos += 2;

  char path[PATH_LEN];
  memset(path, 0, 32);
  char *buf = path;

  while(s[*pos] != ')') {
    *buf++ = s[*pos];
    *pos += 1;
  }
  *pos += 1;
  
  return ctor_link(path);
}


Node* parse(char * s, size_t *pos) {
  if (s[*pos] == '(' && s[*pos+1] == ')') {
    *pos += 2;
    return NULL;
  }

  char name[NAME_LEN + 1];
  memcpy(name, s + *pos, NAME_LEN + 1);

  for (int i = 0; i < NAME_LEN + 1; i++) {
    if (name[i] == ':') {
      *pos += 1;
      memset(name + i, 0, NAME_LEN + 1 - i);
      goto good;
    }
    *pos += 1;
  }
  // bad news, didn't find one.
  printf("Name too long: %s", name);
  assert(0);

good:

  switch(s[*pos]) {
    case 'D':
      return dir2Node(name, parse_directory(s, pos));
    case 'L':
      return link2Node(name, parse_link(s, pos));
    case 'B':
      return block2Node(name, parse_block(s, pos));
    default:
      assert(0);
  }
}
