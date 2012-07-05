#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bcurses.h>
#include <vector.h>
#include <extras.h>

struct textbox {
  canvas* c;
  int yidx;
  int xidx;

  vector* v;
};

static vector* create_line(const char* s) {
  vector* l = make_vector(strlen(s) * 2);

  fori(i, strlen(s)) {
    vpush(l, (void*)s[i]);
  }
  return l;
}

static vector* current_line(textbox* tb) {
  return vget(tb->v, tb->yidx);
}

void register_tb(canvas* c, textbox* tb);

textbox* init_textbox(canvas* c, const char * s) {
  textbox* tb = malloc(sizeof(textbox));
  tb->c = c;
  tb->yidx = 0;
  tb->xidx = 0;

  register_tb(c, tb);

  vector* v = split(s, "\n");
  tb->v = map(char*, s, v, create_line(s));
  cleanup_vector(v);

  if (vsize(tb->v) == 0) {
    vector* l = create_line("  ");
    vpush(tb->v, l);
  }
  return tb;
}

bool insert(textbox* tb, int ch) {
  vector* l = current_line(tb);
  vinsert(l, tb->xidx++, (void*)ch);
  return true;
}

bool backspace(textbox* tb) {
  if (tb->xidx == 0) return false;

  vector* l = current_line(tb);
  vremove(l, tb->xidx--);
  return true;
}

bool newline(textbox* tb) {
  return false;

  vector *old = current_line(tb);
  vector* l = create_line("");

  while((unsigned)tb->xidx < vsize(old)) {
    vpush(l, vget(old, tb->xidx));
    vremove(old, tb->xidx);
  }

  tb->yidx++;
  tb->xidx = 0;
  vinsert(tb->v, tb->yidx + 1, l);
  return true;
}

bool get_text(textbox* tb, char* buf, size_t size) {
  int pos = 0;
  fori(i, vsize(tb->v)) {
    each(char, c, vget(tb->v, i), buf[pos++] = c);
    buf[pos++] = '\n';
  }
  buf[pos++] = '\0';
  return true;
}

int RATTR(canvas* r, int x, int y, unsigned attr);

void draw_tb(textbox* tb) {
  blank(tb->c);
  if (vsize(tb->v) == 0) return;

  char buf[4096];
  int pos = 0;

  fori(i, vsize(tb->v)) {
    pos = 0;
    each(char, c, vget(tb->v, i), buf[pos++] = c);
    buf[pos++] = '\0';
    
    draw_text_nowrap(tb->c, 0, i, buf);
  }
  RATTR(tb->c, tb->xidx, tb->yidx, A_UNDERLINE);
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
  switch(ch) {
    case '\n':
      return newline(tb);
    case KEY_BACKSPACE:
      return backspace(tb);
    case KEY_LEFT:
      tb->xidx = MAX(0, tb->xidx - 1);
      return true;
    case KEY_RIGHT:
      tb->xidx = MIN(vsize(current_line(tb)) - 1, (unsigned ) tb->xidx + 1);
      return true;
    case KEY_UP:
      tb->yidx = MAX(0, tb->yidx - 1);
      tb->xidx = MIN(vsize(current_line(tb)), (unsigned)tb->xidx);
      return true;
    case KEY_DOWN:
      tb->yidx = MIN((int)vsize(tb->v) - 1, tb->yidx + 1);
      tb->xidx = MIN(vsize(current_line(tb)), (unsigned)tb->xidx);
      return true;
    default:
      return false;
  }
}

void dtor_line(vector* l) {
  cleanup_vector(l);
}

void dtor_textbox(textbox* tb) {
  if (!tb) return;

  each(vector*, l, tb->v, dtor_line(l));
  cleanup_vector(tb->v);
  free(tb);
}

