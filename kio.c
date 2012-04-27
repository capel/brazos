#include "stdlib.h"
#include "kio.h"
#include "syscalls.h"


#define DEV_CONS_ADDRESS        0x0000000010000000
#define DEV_CONS_LENGTH         0x0000000000000020
#define     DEV_CONS_PUTGETCHAR         0x0000
#define     DEV_CONS_HALT           0x0010
#define	PHYSADDR_OFFSET		0


#define	PUTCHAR_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_PUTGETCHAR)
#define	HALT_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_HALT)
int vprintf(char* buf, size_t size, const char* fmt, va_list va);

char kgetch(void)
{
    char ch = *((volatile unsigned char *) PUTCHAR_ADDRESS) ;
	return ch;
}

void halt(void) 
{
	*((volatile unsigned char *) HALT_ADDRESS) = 0;
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
    vprintf(buf, 1024, fmt, va);
    va_end(va);

    kputs(buf);

    return 0;
}

int printf(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va);
    va_end(va);


    syscall(WRITE_STDOUT, (int)buf, 0, 0);

    return 0;
}



int snprintf(char* buf, size_t size, const char* fmt, ...)
{
    if (size == 0)
        return -1;

    va_list va;
    va_start(va, fmt);
    vprintf(buf, size, fmt, va);
    va_end(va);

    return 0;
}



int vprintf(char* buf, size_t size, const char* fmt, va_list va)
{
    const int BUFSIZE = 64;
    char tmpbuf[BUFSIZE]; // PLENTY of room for scratch stuff
    int chars, d;
    unsigned u;
    char *s;

    size_t fmtpos, bufpos;
    for (fmtpos = 0, bufpos = 0; bufpos < size && fmt[fmtpos] != '\0'; fmtpos++) {
        if (fmt[fmtpos] != '%') {
            buf[bufpos] = fmt[fmtpos];
            bufpos++;
        } else {
            fmtpos++;
            switch (fmt[fmtpos]) {
                case 'd':
                    d = va_arg(va, int);
                    itoa(tmpbuf, BUFSIZE, d);
                    chars = strlcpy(buf + bufpos, tmpbuf, size - bufpos);
                    bufpos += chars -1; // remove null
                    break;
                case 'u':
                    u = va_arg(va, unsigned);
                    utoa(tmpbuf, BUFSIZE, u);
                    chars = strlcpy(buf + bufpos, tmpbuf, size - bufpos);
                    bufpos += chars -1; // remove null
                    break;
                case 'p': // we assume sizeof(void*) = sizeof(unsigned)
                case 'x':
                    u = va_arg(va, unsigned);
                    utoa16(tmpbuf, BUFSIZE, u);
                    chars = strlcpy(buf + bufpos, tmpbuf, size - bufpos);
                    bufpos += chars -1; // remove null
                    break;
                case 's':
                    s = va_arg(va, char*);
                    chars = strlcpy(buf + bufpos, s, size - bufpos);
                    bufpos += chars -1; // remove null
                    break;
                case 'c':
                    d = va_arg(va, int);
                    buf[bufpos] = (char)d;
                    bufpos++;
                    break;
                case '%':
                    buf[bufpos] = '%';
                    bufpos++;
                    break;
            }
        }
    }

    if (bufpos >= size) {
        buf[size-1] = '\0';
        return -1;
    } else {
        buf[bufpos] = '\0'; // null-terminate regardless
        return 0;
    }
}
