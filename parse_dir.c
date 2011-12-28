#define USER

#include "parse_dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "types.h"
#include "vector.h"


// format: {name:<#,C>,name2:<#,C>}
// name can be any character except: , space / :
// newlines and \0 is not allowed anywhere except after }
// this format is _strictly_ enforced. No accepting parsers here.

#define CONSUME(c) if (*(s++) != c) return 0


static const char* get_name(const char* s, char* buf) {
  // this could tots overflow buf...
  while (true) {
    switch (*s) {
      case '\0':
      case ' ':
      case ',':
      case '/':
        return 0;
      case ':':
        *(++buf) = '\0';
        s++;
        return s;
      default:
        *(buf++) = *(s++);
        break;
    }
  }
}

static const char* get_num(const char* s, int* num) {
  char _buf[32];
  char* buf = _buf;

  while (true) {
    switch (*s) {
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
        *(buf++) = *(s++);
        break;
      case ',':
        *(buf) = '\0';
        *num = atoi(_buf);
        return s;
      default:
        return 0;
    }
  }
}

static const char* get_type(const char* s, int* type) {
  switch (*s) {
    case 'F':
    case 'M':
    case 'D':
    case 'U':
    case 'S':
    case 'B':
      *type = *s;
      return ++s;
    default:
      return 0;
  }
}

static const char* get_rid(const char* s, int *rid, int* type) {
  CONSUME('<');

  s = get_num(s, rid);
  if (!s) return 0;

  CONSUME(',');

  s = get_type(s, type);
  if (!s) return 0;
  
  CONSUME('>');
  return s;
}

static const char* next_de(const char* s, dir_entry *out) {
  s = get_name(s, out->name);
  if (!s) return 0;

  s = get_rid(s, &out->rid, &out->type);
  if (!s) return 0;

  // there is no comma at the end
  if (*s == '}') return s;

  CONSUME(',');
  return s;
}


parsed_dir* parse_dir(const char* s) {
  parsed_dir * dir = malloc(sizeof(parsed_dir));
  dir->size = 0;

  vector *v = kmake_vector(sizeof(dir_entry*), UNMANAGED_POINTERS);

  s = skip_spaces(s); // the only place we allow extra spaces
  if (*(s++) != '{') goto fail;

  while(*s && *s != '}') {
    dir_entry * e = malloc(sizeof(dir_entry));
    s = next_de(s, e);
    if (!s) goto fail;
    vector_push(v, (char*)e);
    dir->size++;
  }

  if (*s != '}') goto fail;

  dir->entries = malloc(dir->size * sizeof(dir_entry*));
  memcpy((char*)dir->entries, (char*)v->data, dir->size * sizeof(dir_entry*)); // heh
  cleanup_vector(v);

  return dir;

fail:
  println("FAIL");
  free(dir);
  cleanup_vector(v);
  return 0;
}

void cleanup_parsed_dir(parsed_dir* d) {
  for(size_t i = 0; i < d->size; i++) {
    free(d->entries[i]);
  }
  free(d->entries);
  free(d);
}
