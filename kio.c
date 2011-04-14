#include "stdlib.h"
#include "kio.h"
#include "syscalls.h"
#include "chars.h"

#define DEV_CONS_ADDRESS        0x0000000010000000
#define DEV_CONS_LENGTH         0x0000000000000020
#define     DEV_CONS_PUTGETCHAR         0x0000
#define     DEV_CONS_HALT           0x0010
#define	PHYSADDR_OFFSET		0


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

void _printk(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
    va_end(va);

    kputs(buf);

}


void _panic(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
    va_end(va);

    kputs(buf);
    khalt();
}

int printf(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, false);
    va_end(va);


    syscall(WRITE_STDOUT, (int)buf, 0, 0);

    return 0;
}


int println(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
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
    vprintf(buf, size, fmt, va, false);
    va_end(va);

    return 0;
}



int vprintf(char* buf, size_t size, const char* fmt, va_list va, int newline)
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
                    //bufpos += strlcpy(buf+bufpos, GREEN, strlen(GREEN));
                    //bufpos--;
                    bufpos += strlcpy(buf + bufpos, tmpbuf, size - bufpos);
                    bufpos--; // remove null
                    //bufpos += strlcpy(buf+bufpos, WHITE, strlen(WHITE));
                    //bufpos--;
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

    if (bufpos >= size-1) {
        if (newline)
            buf[size-2] = '\n';
        
        buf[size-1] = '\0';
        return -1;
    } else {
        if (newline)
            buf[bufpos++] = '\n';
        buf[bufpos] = '\0'; // null-terminate regardless
        return 0;
    }
}
