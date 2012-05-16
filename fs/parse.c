#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "../chars.h"

#define CONSUME(c) do { if(s[*pos] != c) { DIE(); } (*pos)++; } while (0)
#define NOM() s[(*pos)++]

#define NOM_SPACE() while (isspace(s[*pos])) { (*pos)++; }

#define PARSE(name) name(const char * s, size_t *pos, bool* die)

PARSE(_parse);

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#define DIE() do { *die = true; printk("Parse error\n"); return 0; } while(0)

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
    char *s = malloc(1);
    s[0] = '\0';
    return s;
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
  Block* b =  ctor_block(i);

  NOM_SPACE();
  CONSUME(')');

  return b;
}

static Link* PARSE(link_parse) {
  CONSUME('L');
  CONSUME('(');
  NOM_SPACE();
  int i = str_parse(s, pos, die);
  Link* l =  ctor_link(i);

  NOM_SPACE();
  CONSUME(')');
  return l;
}

static Directory* PARSE(dir_parse) {
  CONSUME('D');
  CONSUME('(');
  NOM_SPACE();
  Block * b = block_parse(s, pos, die);
  Directory* d =  ctor_directory(b);

  NOM_SPACE();
  CONSUME(')');
  return d;
}

static Entry* PARSE(entry_prase) {
  CONSUME('E');
  CONSUME('(');
  NOM_SPACE();

  const char * s = str_parse(s, pos, die);
  NOM_SPACE();
  Node * n = node_parse(s, pos, die);

  Entry* e = ctor_entry(s, n);

  NOM_SPACE();
  CONSUME(')');
  return e;
}

static int file_parse(const char *s, size_t *pos, bool* die) {
  int num_args = 0;
  Block args[MAX_ARGS];
  memset(args, 0, sizeof(args));

  int i = int_parse(s, pos, die);
  for(;;) {
    HOPE(num_args < MAX_ARGS);
    args[num_args++] = block_parse(s, pos, die);
    NOM_SPACE();
  }

  File* f = ctor_file(i, args, num_args);

  NOM_SPACE();
  CONSUME(')');
  return f;
}

static Node* PARSE(node_parse) {
  switch(s[*pos]) {
    case 'L':
      return NODE(parse_link(s, pos, die));
    case 'E':
      return NODE(parse_entry(s, pos, die));
    case 'D':
      return NODE(parse_directory(s, pos, die));
    default:
      DIE();
  }
}

Entry* parse_dir_block(const char * s, size_t *num_args) {
  bool die = 0;
  size_t pos = 0;
  *num_args = 0;
  
  Entry* args[MAX_ARGS];
  memset(args, 0, sizeof(args));

  for(;;) {
    HOPE(*num_args < MAX_ARGS);
    args[num_args++] = entry_parse(s, pos, die);
    NOM_SPACE();
  }

  return args;
}
