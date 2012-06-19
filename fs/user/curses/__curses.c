#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int readch() {
  return getch();
}

void setup() {
  initscr();
  raw();
  noecho();
}

void teardown() {
  endwin();
  fflush(0);
}

static int linearize(int max_x, int max_y, int x, int y) {
  assert(x >= 0 && x < max_x && y >= 0 && y < max_y);
  return y * max_x + x;
}

void _blit(int* buf, int max_x, int max_y) {
  for(int y = 0; y < max_y; y++) {
    for(int x = 0; x < max_x; x++) {
      mvaddch(y, x, (char) buf[linearize(max_x, max_y, x, y)]);
    }
    //putchar('\n');
  }
  refresh();
}
