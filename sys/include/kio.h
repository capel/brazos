#ifndef KIO_H
#define KIO_H

#include "../../common/chars.h"
#include "../../common/types.h"

#ifdef USER
#error __file__ " included from user code."
#endif

int kgetc();
void khalt() __attribute__((noreturn));

void kputs(const char *s);
void kputc(int);

int kprintf(const char* fmt, ...);
int _printk(const char* fmt, ...);
int _panic(const char* fmt, ...) __attribute__((noreturn));

const char* kitoa(int i);
const char* kstrclone(const char* s);

#define panic(x, args...) _panic(RED "PANIC " WHITE  __FILE__ ":%d [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#define printk(x, args...) _printk(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#endif
