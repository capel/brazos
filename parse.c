#include "variant.h"
#include "vv.h"
#include "stdlib.h"

#include "parse_macro.h"


static int get_int(const char * s, size_t *pos, bool* die) {
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

static PARSE(int_parse) {
  return Int(get_int(s, pos, die));
}

static PARSE(handle_parse) {
  CONSUME('<');
  NOM_SPACE();
  (*pos)++; // nom subtype
  NOM_SPACE();
  int i = get_int(s, pos, die);
  NOM_SPACE();
  CONSUME('>');

  return Handle((void*)i, H_H);
}

static PARSE(str_parse) {
  CONSUME('"');
  if (s[*pos] == '"') {
    CONSUME('"');
    return Str("");
  }
  size_t len = 0;
  while(s[*pos+len] != '"') { len++; }
  char * b = malloc(len + 2);
  memcpy(b, s + *pos, len);
  b[len] = '\0';
  *pos += len;

  if (NOM() != '"') {
    free(b);
    DIE();
  }
  return RStr(b);
}

PARSE(null_parse) {
  CONSUME('n');
  CONSUME('u');
  CONSUME('l');
  CONSUME('l');
  return Null();
}

PARSE(call_parse);
PARSE(map_parse);

PARSE(_parse) {
  NOM_SPACE();

  switch (s[*pos]) {
    case '[': 
      return call_parse(s, pos, die);
    case '{':
      return map_parse(s, pos, die);
    case 'n':
      return null_parse(s, pos, die);
    //case '<':
    //  return handle_parse(s, pos, die);
    //case '(':
    //  return call_parse(s, pos, die);
    case '"':
      return str_parse(s, pos, die);
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
    case '-':
      return int_parse(s, pos, die);
    default:
      DIE();
  }
}

variant parse(const char * s) {
  bool die = 0;
  size_t pos = 0;
  variant v = _parse(s, &pos, &die);
  if (s[pos]) {
    printk("Parse error: trailing bullshit %s", s + pos);
  }
  return v;
}
