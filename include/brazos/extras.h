#ifndef EXTRAS_H
#include "../../chars.h"
#include <stdio.h>

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x "\n", __LINE__, __func__, ## args)

char* strclone(const char* s);
void rtrim(char* s);

int Printf(const char* fmt, ...);

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#endif
