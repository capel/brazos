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

#define PARSE(name) Node* name(const char * s, size_t *pos, bool* die)

PARSE(_parse);

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#define DIE() do { *die = true; printk("Parse error\n"); return 0; } while(0)

#define HOPE(cond) if (!(cond)) { DIE(); }


static PARSE(int_parse) {
  char buf[64];
  size_t bufpos = 0;
  if (s[*pos] == '-') {
    buf[bufpos++] = NOM();
  }

  while(isdigit(s[*pos])) {
    buf[bufpos++] = NOM(); 
  }
  buf[bufpos++] = '\0';

  return NODE(atoi(buf));
}

static PARSE(str_parse) {
  CONSUME('\'');
  if (s[*pos] == '\'') {
    CONSUME('\'');
    char *s = malloc(1);
    s[0] = '\0';
    return NODE(s);
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
  return NODE(b);
}

static int argslist_parse(const char *s, size_t *pos, bool* die, Node** args) {
  CONSUME('(');

  NOM_SPACE()

  int num_args = 0;

  for(;;) {
    HOPE(num_args < MAX_ARGS);
    args[num_args++] = _parse(s, pos, die);

    NOM_SPACE();

    char c = NOM();
    if (c == ')') {
      return num_args;
    } else if (c ==',') {
      NOM_SPACE();
      if (s[*pos] == ')') {
        *pos += 1;
        return num_args;
      }
      continue;
    } else {
      printf("char: %c (%d)\n", c, c);
      DIE();
    }
  }
}

static PARSE(ctor_parse) {
  char ctor = s[*pos];
  *pos += 1;

  Node* args[MAX_ARGS];
  memset(args, 0, sizeof(args));

  int num_args = argslist_parse(s, pos, die, args);

  switch(ctor) {
    case 'D':
      return NODE(ctor_directory(args));
    case 'B':
      HOPE(num_args == 1 && args[0]->type == INTEGER);
      return NODE(ctor_block(args[0]));
    case 'L':
      HOPE(num_args == 1 || args[0]->type == STRING);
      return NODE(ctor_link(args[0]));
    case 'E':
      HOPE(num_args == 2 || args[0]->type == STRING);
      return NODE(ctor_entry(args[0], args[1]));
    default:
      DIE();
  }
}

PARSE(_parse) {
  NOM_SPACE();

  switch (s[*pos]) {
    case '\'':
      return str_parse(s, pos, die);
    case '0' ... '9':
    case '-':
      return int_parse(s, pos, die);
    case 'D':
    case 'B':
    case 'L':
    case 'E':
      return ctor_parse(s, pos, die);
    default:
      printf("%c (%d)\n", s[*pos], s[*pos]);
      DIE();
  }
}

Node* parse(const char * s) {
  bool die = 0;
  size_t pos = 0;
  Node* n = _parse(s, &pos, &die);
  if (s[pos] && s[pos] != '\n') {
    printf("Parse error: trailing bullshit \"%s\"\n", s + pos);
  }
  return n;
}
