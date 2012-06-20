#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <bcurses.h>

int readch() {
  return getch();
}

void setup() {
  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
}

void teardown() {
  endwin();
  fflush(0);
}

static int linearize(int max_x, int max_y, int x, int y) {
  assert(x >= 0 && x < max_x && y >= 0 && y < max_y);
  return y * max_x + x;
}

void _blit(glyph* buf, int max_x, int max_y) {
  for(int y = 0; y < max_y; y++) {
    for(int x = 0; x < max_x; x++) {
      glyph g = buf[linearize(max_x, max_y, x, y)];
      if (g.attr != A_NORMAL) {
        attron(g.attr);
        mvaddch(y, x, (char)g.ch);
        attroff(g.attr);
      } else {
        mvaddch(y, x, (char)g.ch);
      }
    }
  }
  refresh();
}
