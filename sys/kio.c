#include "include/common.h"
#include "../common/stdlib.h"
#include "include/kio.h"
#include "../common/syscalls.h"
#include "../common/chars.h"

#define DEV_CONS_ADDRESS        0x0000000010000000
#define DEV_CONS_LENGTH         0x0000000000000020
#define     DEV_CONS_PUTGETCHAR         0x0000
#define     DEV_CONS_HALT           0x0010
#define	PHYSADDR_OFFSET		0

const char* kitoa(int i) {
  char* s = kmalloc(12);
  snprintf(s, 12, "%d", i);
  return s;
}

#define	PUTCHAR_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_PUTGETCHAR)
#define	HALT_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_HALT)
int vprintf(char* buf, size_t size, const char* fmt, va_list va, int newline);

int kgetc(void)
{
    char ch = *((volatile unsigned char *) PUTCHAR_ADDRESS) ;
	return ch;
}

void khalt(void) 
{
	*((volatile unsigned char *) HALT_ADDRESS) = 0;
}

void kputc(int c) {
    if (isspecial(c)) {
        kputc(27);
        kputc(91);
        switch (c) {
            case ARROW_LEFT:
                kputc('D');
                break;
            case ARROW_RIGHT:
                kputc('C');
                break;
            case ARROW_UP:
                kputc('A');
                break;
            case ARROW_DOWN:
                kputc('B');
                break;
            default:
                printk("Invalid special char %d sent to kputc.", c);
                break;
        }
        return;
    }
    *((volatile unsigned char *) PUTCHAR_ADDRESS) = c;
}

void kputs(const char *s)
{
	//kputch('<');
	while (*s) {
	    *((volatile unsigned char *) PUTCHAR_ADDRESS) = *s++;
	}
	*((volatile unsigned char *) PUTCHAR_ADDRESS) = '\0';
}


int kprintf(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, false);
    va_end(va);

    kputs(buf);

    return 0;
}

int _printk(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
    va_end(va);

    kputs(buf);
    return 0;
}


int _panic(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
    va_end(va);

    kputs(buf);
    khalt();
    return 0;
}
