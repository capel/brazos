#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include <vector.h>
#include <extras.h>

#define CONSUME(c) do { if(s[*pos] != c) { DIE(); } (*pos)++; } while (0)
#define NOM() s[(*pos)++]

#define NOM_SPACE() while (isspace(s[*pos])) { (*pos)++; }

#define PARSE(name) name(const char * s, size_t *pos, bool* die)
#define DIE() do { *die = true; printk("Parse error at pos %lu [[%s]]\n", *pos, s); return 0; } while(0)

#define HOPE(cond) if (!(cond)) { DIE(); }


static int PARSE(int_parse) {
  char buf[64];
  size_t bufpos = 0;
  if (s[*pos] == '-') {
    buf[bufpos++] = NOM();
  }

  while(isdigit(s[*pos])) {
    buf[bufpos++] = NOM(); 
  }
  buf[bufpos++] = '\0';

  return atoi(buf);
}

static const char* PARSE(str_parse) {
  CONSUME('\'');
  if (s[*pos] == '\'') {
    CONSUME('\'');
    return strclone("");
  }
  size_t len = 0;
  while(s[*pos+len] != '\'') { len++; }
  char * b = malloc(len + 2);
  memcpy(b, s + *pos, len);
  b[len] = '\0';
  *pos += len;

  if (NOM() != '\'') {
    free(b);
    DIE();
  }
  return b;
}

static Block* PARSE(block_parse) {
  CONSUME('B');
  CONSUME('(');
  NOM_SPACE();
  int i = int_parse(s, pos, die);
  Block* b = block_ctor(i);

  NOM_SPACE();
  CONSUME(')');

  return b;
}

static Link* PARSE(link_parse) {
  CONSUME('L');
  CONSUME('(');
  NOM_SPACE();
  const char * ls = str_parse(s, pos, die);
  Link* l =  link_ctor(ls);

  NOM_SPACE();
  CONSUME(')');
  return l;
}

static Directory* PARSE(dir_parse) {
  CONSUME('D');
  CONSUME('(');
  NOM_SPACE();
  Block * b = block_parse(s, pos, die);
  Directory* d =  dir_ctor(b);

  NOM_SPACE();
  CONSUME(')');
  return d;
}

static Node* PARSE(node_parse);

static Entry* PARSE(entry_parse) {
  CONSUME('E');
  CONSUME('(');
  NOM_SPACE();

  const char * ls = str_parse(s, pos, die);
  NOM_SPACE();
  Node * n = node_parse(s, pos, die);

  Entry* e = entry_ctor(ls, n);
  free((char*)ls);

  NOM_SPACE();
  CONSUME(')');
  return e;
}


static File* PARSE(file_parse) {
  CONSUME('F');
  CONSUME('(');

  int num_args = 0;
  Block* args[MAX_ARGS];
  memset(args, 0, sizeof(args));

  int len = int_parse(s, pos, die);
  for(;;) {
    HOPE(num_args < MAX_ARGS);
    NOM_SPACE();
    args[num_args++] = block_parse(s, pos, die);
    NOM_SPACE();
    if (s[*pos] == ')') {
      CONSUME(')');
      break;
    }
  }

  File* f = file_ctor(len, args, num_args);
  return f;
}

static Node* PARSE(node_parse) {
  switch(s[*pos]) {
    case 'L':
      return NODE(link_parse(s, pos, die));
    case 'D':
      return NODE(dir_parse(s, pos, die));
    case 'F':
      return NODE(file_parse(s, pos, die));
    default:
      DIE();
  }
}

vector* parse_dir_block(const char * s) {
  vector* v = make_vector(8);

  // empty directory
  if (strlen(s) == 0) {
    return v;
  }

  bool _die = 0;
  bool *die = &_die;
  size_t _pos = 0;
  size_t *pos = &_pos;
  

  for(;;) {
    push(v, entry_parse(s, pos, die));
    NOM_SPACE();
    if (s[*pos] != 'E') break;
  }

  return v;
}
