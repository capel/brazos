#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bcurses.h>

struct scr {
  int x;
  int y;
  int* buf;
};

struct region {
  scr* s;
  int base_x;
  int base_y;
  int size_x;
  int size_y;

  int cur_x;
  int cur_y;
};

static int linearize(scr* s, int x, int y) {
  assert(x >= 0 && x < s->x && y >= 0 && y < s->y);
  return y * s->x + x;
}

int POS(scr* s, int x, int y) {
  return s->buf[linearize(s, x, y)];
}

int PUT(scr* s, int x, int y, int ch) {
  return s->buf[linearize(s, x, y)] = ch;
}

int RPOS(region* r, int x, int y) {
  return POS(r->s, x + r->base_x, y + r->base_y);
}

int RPUT(region* r, int x, int y, int ch) {
  return PUT(r->s, x + r->base_x, y + r->base_y, ch);
}

#define BUFSIZE(s) ((s)->x * (s)->y)
#define REGSIZE(s) ((r->x2 - r->x1) * (r->y2 - r->y1))
#define HLINE '-'
#define VLINE '|' 
#define CROSS '+'

#define INSIDE(_r, _x, _y) ((_x >= 0) && (_x <= _r->size_x) && (_y >= 0) && (_y <= _r->size_y))


void setup();
void teardown();

scr* init_screen(int sx, int sy) {
  setup();

  scr* s = malloc(sizeof(scr));
  s->x = sx;
  s->y = sy;
  s->buf = calloc(sizeof(int), BUFSIZE(s) + 1);
  for(int i = 0; i < sx * sy; i++) {
    s->buf[i] = ' ';
  }
  s->buf[sx * sy] = '\0';

  return s;
}

region* init_region(scr* s, int x1, int y1, int x2, int y2) {
  region* r = malloc(sizeof(region));
  r->s = s; 
  r->base_x = x1;
  r->base_y = y1;
  r->size_x = x2 - x1;
  r->size_y = y2 - y1;

  r->cur_x = 0;
  r->cur_y = 0;
  return r;
}
  
region* full_region(scr* s) {
  return init_region(s, 0, 0, s->x, s->y);
}

void dtor_region(region* r) {
  free(r);
}


void _blit(int* buf, int max_x, int max_y);
void blit(scr* sc) {
  _blit(sc->buf, sc->x, sc->y);
}

void dtor_screen(scr* s) {
  free(s->buf);
  free(s);
  teardown();
}

void draw_line_point(region* r, int x, int y, int p) {
  int other = p == HLINE ? VLINE : HLINE;
  RPUT(r, x, y, RPOS(r, x, y) == other ? CROSS : p);
}

bool draw_vline(region* r, int x, int y1, int y2) {
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
bool draw_hline(region* r, int x1, int x2, int y) {
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

bool draw_box(region* r, int x1, int y1, int x2, int y2) {
  if (!INSIDE(r, x1, y1) || !INSIDE(r, x2, y2)) return false;

  draw_vline(r, x1, y1, y2);
  draw_vline(r, x2, y1, y2);
  draw_hline(r, x1, x2, y1);
  draw_hline(r, x1, x2, y2);
  return true;
}

bool draw_text(region *r, int ix, int iy, const char* buf) {
  int pos = 0;
  for (int y = iy; y < r->size_y; y++) {
    for (int x = ix; x < r->size_x; x++) {
      if (!buf[pos]) {
        move_cur(r, x, y);
        return true;
      };
      RPUT(r, x, y, buf[pos++]);
    }
  }
  return true;
}

int region_to_buffer(region *r, char* buf, size_t size) {
  size_t pos = 0;
  for (int y = 0; y < r->size_y; y++) {
    for (int x = 0; x < r->size_x; x++) {
      if (pos >= size - 1) goto end;
      buf[pos++] = RPOS(r, x, y);
    }
  }

end:
  buf[pos] = '\0';
  return pos;
}

bool move_cur(region* r, int x, int y) {
  if (!INSIDE(r, x, y)) return false;

  r->cur_x = x;
  r->cur_y = y;

  return true;
}

bool writech(region* r, int ch) {
  RPUT(r, r->cur_x, r->cur_y, ch);

  r->cur_x++;
  if (!INSIDE(r, r->cur_x, r->cur_y)) {
    r->cur_x = 0;
    r->cur_y++;
  }

  if (!INSIDE(r, r->cur_x, r->cur_y)) {
    r->cur_y = 0;
  }
  return true;
}

bool backspace(region* r) {
  if (r->cur_x == 0 && r->cur_y == 0) {
    RPUT(r, 0, 0, ' ');
    return true;
  }

  r->cur_x--;
  if (r->cur_x < 0) {
    r->cur_x = r->size_x;
    r->cur_y--;
  }

  if (r->cur_y < 0) {
    r->cur_y = 0;
  }

  RPUT(r, r->cur_x, r->cur_y, ' ');

  return true;
}

bool newline(region* r) {
  if (INSIDE(r, 0, r->cur_y + 1)) {
    r->cur_y++;
    r->cur_x = 0;
    return true;
  }
  return false;
}

bool is_magic(int ch) {
  switch(ch) {
    case '\b':
    case '\n':
      return true;
    default:
      return false;
  }
}

bool standard_magic(region* r, int ch) {
  switch(ch) {
    case '\b':
      return backspace(r);
    case '\n':
      return newline(r);
    default:
      return false;
  }
}
