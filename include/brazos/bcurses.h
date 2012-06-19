#ifndef CURSES_H
#define CURSES_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct scr scr;
typedef struct region region;

scr* init_screen(int sx, int sy);
region* init_region(scr*, int x1, int y1, int x2, int y2);
region* full_region(scr*);

void dtor_screen(scr* s);
void dtor_region(region* r);

void blit(scr* sc);

bool draw_text(region*, int x, int y, const char* buf);
bool draw_vline(region*, int x, int y1, int y2);
bool draw_hline(region*, int x1, int x2, int y);
bool draw_box(region*, int x1, int y1, int x2, int y2);

int readch();

bool move_cur(region*, int x, int y);
bool writech(region*, int ch);
bool backspace(region*);
bool newline(region* r);
int region_to_buffer(region *r, char* buf, size_t size);
#endif
