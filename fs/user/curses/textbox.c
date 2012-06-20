#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bcurses.h>
#include <vector.h>
#include <extras.h>

typedef struct {
  char* buf;
  int size;
  int idx;
} line;

struct textbox {
  canvas* c;
  int idx;

  vector* v;
};

static line* create_line(const char* s) {
  line* l = malloc(sizeof(line));
  l->size = MAX(64, 2 * strlen(s));
  l->buf = malloc(l->size);
  l->idx = 0;

  strcpy(l->buf, s);
  return l;
}

static bool shift(line* l, int start, int amt) {
  if (amt == 0) {
  } else if (amt > 0) {
    int nchars = l->size - start - amt;
    memmove(l->buf + start + amt, l->buf + start, nchars);
  } else {
    int nchars = l->size - start;
    memmove(l->buf + start + amt, l->buf + start, nchars);
    memset(l->buf + start + amt + nchars, '~', nchars);
  }
  return true;
}

static line* current_line(textbox* tb) {
  return (line*) vget(tb->v, tb->idx);
}

void register_tb(canvas* c, textbox* tb);

textbox* init_textbox(canvas* c, const char * s) {
  textbox* tb = malloc(sizeof(textbox));
  tb->c = c;
  tb->idx = 0;

  register_tb(c, tb);

  vector* v = split(s, "\n");
  tb->v = map(char*, s, v, create_line(s));
  cleanup_vector(v);
  return tb;
}

bool insert(textbox* tb, int ch) {
  line* l = current_line(tb);
  shift(l, l->idx, 1);
  l->buf[l->idx++] = ch;
  return true;
}

bool backspace(textbox* tb) {
  line* l = current_line(tb);
  shift(l, l->idx, -1);
  l->idx--;
  return true;
}

bool newline(textbox* tb) {
  line *old = current_line(tb);
  line* l = create_line(old->buf + old->idx);
  old->buf[old->idx] = '\0';

  vinsert(tb->v, tb->idx + 1, l);
  tb->idx++;
  return true;
}

bool get_text(textbox* tb, char* buf, size_t size) {
  vector * v = map(line*, l, tb->v, l->buf);
  each(char*, s, v, s[strlen(s) -1] = '\n');

  char* joined = join(v, "");
  cleanup_vector(v);

  strncpy(buf, joined, size);
  buf[size-1] = '\0';

  free(joined);
  return true;
}

int RATTR(canvas* r, int x, int y, unsigned attr);

void draw_tb(textbox* tb) {
  blank(tb->c);
  fori(i, vsize(tb->v)) {
    draw_text_nowrap(tb->c, 0, i, ((line*)vget(tb->v, i))->buf);
  }
  RATTR(tb->c, current_line(tb)->idx, tb->idx, A_UNDERLINE);
}

bool is_magic(int ch) {
  switch(ch) {
    case KEY_BACKSPACE:
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_UP:
    case KEY_DOWN:
    case '\n':
      return true;
    default:
      return false;
  }
}

bool standard_magic(textbox* tb, int ch) {
  line * l = current_line(tb);
  switch(ch) {
    case '\n':
      return newline(tb);
    case KEY_BACKSPACE:
      return backspace(tb);
    case KEY_LEFT:
      l->idx = MAX(0, l->idx - 1);
      return true;
    case KEY_RIGHT:
      l->idx = MIN((int)strlen(l->buf), l->idx + 1);
      return true;
    case KEY_UP:
      tb->idx = MAX(0, tb->idx - 1);
      return true;
    case KEY_DOWN:
      tb->idx = MIN((int)vsize(tb->v) - 1, tb->idx + 1);
      return true;
    default:
      return false;
  }
}

void dtor_line(line* l) {
  free(l->buf);
  free(l);
}

void dtor_textbox(textbox* tb) {
  each(line*, l, tb->v, dtor_line(l));
  cleanup_vector(tb->v);
  free(tb);
}

