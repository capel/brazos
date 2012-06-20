#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bcurses.h>

struct scr {
  int x;
  int y;
  canvas* canvas_list;
  glyph* buf;
};

#define BUFSIZE(s) ((s)->x * (s)->y)
#define REGSIZE(s) ((r->x2 - r->x1) * (r->y2 - r->y1))

static int linearize(scr* s, int x, int y) {
  assert(x >= 0 && x < s->x && y >= 0 && y < s->y);
  return y * s->x + x;
}

int POS(scr* s, int x, int y) {
  return s->buf[linearize(s, x, y)].ch;
}

int PUT(scr* s, int x, int y, int ch) {
  return s->buf[linearize(s, x, y)].ch = ch;
}

int ATTR(scr* s, int x, int y, unsigned  attr) {
  return s->buf[linearize(s, x, y)].attr = attr;
}

void setup();
void teardown();

scr* init_screen(int sx, int sy) {
  setup();

  scr* s = malloc(sizeof(scr));
  s->x = sx;
  s->y = sy;
  s->buf = calloc(sizeof(glyph), sx * sy + 1);
  for(int i = 0; i < sx * sy; i++) {
    s->buf[i].ch = ' ';
    s->buf[i].attr = A_NORMAL;
  }

  s->canvas_list = 0;

  return s;
}

void register_next(canvas* c, canvas* next);
void register_canvas(scr* s, canvas* c) {
  register_next(c, s->canvas_list);
  s->canvas_list = c;
}

canvas* full_canvas(scr* s) {
  return init_canvas(s, 0, 0, s->x - 1, s->y - 1);
}
  
void draw(canvas* c);

void _blit(glyph* buf, int max_x, int max_y);
void blit(scr* sc) {
  draw(sc->canvas_list);
  _blit(sc->buf, sc->x, sc->y);
}

void dtor_canvas(canvas* c);
void dtor_screen(scr* s) {
  dtor_canvas(s->canvas_list);
  free(s->buf);
  free(s);
  teardown();
}

