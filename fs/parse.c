#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Directory* parse_directory(char *s, size_t *pos) {
  assert(s[*pos] == 'D');
  *pos += 2;

  Directory* dir = ctor_directory();

  dir->nodes[0] = parse(s, pos);
  assert(s[*pos] == ',');
  *pos += 1;
  
  dir->nodes[1] = parse(s, pos);
  assert(s[*pos] == ',');
  *pos += 1;

  dir->nodes[2] = parse(s, pos);
  assert(s[*pos] == ',');
  *pos += 1;

  dir->nodes[3] = parse(s, pos);

  *pos += 1;
  return dir;
}

Block* parse_block(char *s, size_t *pos) {
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


Link* parse_link(char *s, size_t *pos) {
  //printf("l:: %s", s + *pos);
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

  char name[NAME_LEN];
  char* buf = name;
  memset(name, 0, NAME_LEN);

  while(s[*pos] != ':') {
    *buf++ = s[*pos];
    *pos += 1;
  }
  *pos += 1; // eat the :

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
