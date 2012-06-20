#ifndef BCURSES_H
#define BCURSES_H

#include <stdbool.h>
#include <stdlib.h>

#define NCURSES_ATTR_SHIFT       8
#define NCURSES_BITS(mask,shift) ((mask) << ((shift) + NCURSES_ATTR_SHIFT))
#define A_NORMAL	(1U - 1U)
#define A_UNDERLINE	NCURSES_BITS(1U,9)
#define A_REVERSE	NCURSES_BITS(1U,10)

#define KEY_DOWN	0402		/* down-arrow key */
#define KEY_UP		0403		/* up-arrow key */
#define KEY_LEFT	0404		/* left-arrow key */
#define KEY_RIGHT	0405		/* right-arrow key */
#define KEY_ENTER	0527		/* enter/send key */
#define KEY_BACKSPACE	0407		/* backspace key */

typedef struct glyph {
  int ch;
  unsigned int attr;
} glyph;

typedef struct scr scr;
typedef struct canvas canvas;
typedef struct textbox textbox;

scr* init_screen(int sx, int sy);
canvas* init_canvas(scr*, int x1, int y1, int x2, int y2);
canvas* full_canvas(scr*);
textbox* init_textbox(canvas* c, const char* data);

void dtor_screen(scr* s);
void blit(scr* sc);

bool blank(canvas* c);
bool draw_text(canvas*, int x, int y, const char* buf);
bool draw_text_nowrap(canvas *r, int x, int y, const char* buf);
bool draw_vline(canvas*, int x, int y1, int y2);
bool draw_hline(canvas*, int x1, int x2, int y);
bool draw_box(canvas*, int x1, int y1, int x2, int y2);

bool is_magic(int ch);
bool standard_magic(textbox*, int ch);

bool get_text(textbox* tb, char* buf, size_t size);

int readch();
bool insert(textbox*, int ch);
bool backspace(textbox*);
bool newline(textbox* r);
#endif
