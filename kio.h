#ifndef KIO_H
#define KIO_H

#include "chars.h"

#ifdef USER
#error __file__ " included from user code."
#endif

#define DEV_CONS_ADDRESS        0x0000000010000000
#define DEV_CONS_LENGTH         0x0000000000000020
#define     DEV_CONS_PUTGETCHAR         0x0000
#define     DEV_CONS_HALT           0x0010
#define	PHYSADDR_OFFSET		0


#define	PUTCHAR_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_PUTGETCHAR)
#define	HALT_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_HALT)
int kgetc();
void khalt() __attribute__((noreturn));

void kputs(const char *s);
void kputc(int);
int snprintf(char* buf, size_t size, const char* fmt, ...);

inline static void rkputc(int c) {
  *((volatile unsigned char *) PUTCHAR_ADDRESS) = c;
}


int kprintf(const char* fmt, ...);
void _printk(const char* fmt, ...);
void _panic(const char* fmt, ...) __attribute__((noreturn));

const char* kitoa(int i);
char* kstrclone(const char* s);

int strrcpy(char* buf, size_t bufpos, size_t size, const char* s);



#define panic(x, args...) _panic(RED "PANIC " WHITE  __FILE__ ":%d [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#define printk(x, args...) _printk(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#endif
