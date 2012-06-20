#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bcurses.h>
#define INSIDE(_r, _x, _y) ((_x >= 0) && (_x <= _r->size_x) && (_y >= 0) && (_y <= _r->size_y))
#define HLINE '-'
#define VLINE '|' 
#define CROSS '+'

struct canvas {
  scr* s;
  int base_x;
  int base_y;
  int size_x;
  int size_y;
  textbox * tb;
  struct canvas * next;
};

void register_canvas(scr* s, canvas* c);
void register_next(canvas * c, canvas* next) {
  c->next = next;
}

canvas* init_canvas(scr* s, int x1, int y1, int x2, int y2) {
  canvas* r = malloc(sizeof(canvas));
  r->s = s; 
  r->base_x = x1;
  r->base_y = y1;
  r->size_x = x2 - x1;
  r->size_y = y2 - y1;

  r->next = 0;
  register_canvas(s, r);
  return r;
}

void register_tb(canvas* c, textbox* tb) {
  c->tb = tb;
}

int POS(scr* s, int x, int y);
int RPOS(canvas* r, int x, int y) {
  return POS(r->s, x + r->base_x, y + r->base_y);
}

int PUT(scr* s, int x, int y, int ch);
int RPUT(canvas* r, int x, int y, int ch) {
  return PUT(r->s, x + r->base_x, y + r->base_y, ch);
}

int ATTR(scr* s, int x, int y, unsigned attr);
int RATTR(canvas* r, int x, int y, unsigned attr) {
  return ATTR(r->s, x + r->base_x, y + r->base_y, attr);
}

void draw_tb(textbox*);
void draw(canvas* c) {
  if (!c) return;

  if (c->tb) {
    draw_tb(c->tb);
  }
  draw(c->next);
}

bool blank(canvas* r) {
  for(int y = 0; y <= r->size_y; y++) {
    for(int x = 0; x <= r->size_x; x++) {
      RPUT(r, x, y, ' ');
      RATTR(r, x, y, A_NORMAL);
    }
  }
  return true;
}

bool forwards(canvas* c, int x, int y, int* _x, int* _y) {
  x++;
  if (!INSIDE(c, x, y)) {
    x = 0;
    y++;
  }

  if (!INSIDE(c, x, y)) {
    return false;
  }
  *_x = x;
  *_y = y;
  return true;
}

bool draw_text_nowrap(canvas *r, int x, int y, const char* buf) {
  size_t pos = 0;
  while(buf[pos]) {
    if (!INSIDE(r, x, y)) return false;
    RPUT(r, x, y, buf[pos++]);
    x++;
  }

  return true;
}

bool draw_text(canvas *r, int ix, int iy, const char* buf) {
  size_t pos = 0;
  int x = ix;
  int y = iy;
  while(buf[pos]) {
    if (!forwards(r, x, y, &x, &y)) return false;
    RPUT(r, x, y, buf[pos++]);
  }

  return true;
}


void draw_line_point(canvas* r, int x, int y, int p) {
  int other = p == HLINE ? VLINE : HLINE;
  RPUT(r, x, y, RPOS(r, x, y) == other ? CROSS : p);
}

bool draw_vline(canvas* r, int x, int y1, int y2) {
  int min = y1 < y2 ? y1 : y2;
  int max = y1 > y2 ? y1 : y2;

  if (!INSIDE(r, x, min) || !INSIDE(r, x, max)) return false;

  if (y1 == y2) {
    draw_line_point(r, x, y1, VLINE);
    return true;
  }

  for(int y = min; y <= max; y++) {
    draw_line_point(r, x, y, VLINE);
  }

  return true;
}
bool draw_hline(canvas* r, int x1, int x2, int y) {
  int min = x1 < x2 ? x1 : x2;
  int max = x1 > x2 ? x1 : x2;

  if (!INSIDE(r, min, y) || !INSIDE(r, max, y)) return false;

  if (x1 == x2) {
    draw_line_point(r, x1, y, HLINE);
    return true;
  }

  for(int x = min; x <= max; x++) {
    draw_line_point(r, x, y, HLINE);
  }

  return true;
}

bool draw_box(canvas* r, int x1, int y1, int x2, int y2) {
  if (!INSIDE(r, x1, y1) || !INSIDE(r, x2, y2)) return false;

  draw_vline(r, x1, y1, y2);
  draw_vline(r, x2, y1, y2);
  draw_hline(r, x1, x2, y1);
  draw_hline(r, x1, x2, y2);
  return true;
}


void dtor_textbox(textbox*);
void dtor_canvas(canvas* c) {
  if (!c) return;

  dtor_canvas(c->next);
  dtor_textbox(c->tb);
  free(c);
}
