#include "stdlib.h"
#include "kio.h"
#include "syscalls.h"
#include "chars.h"
#include "mem.h"
#include "sys/ko.h"
#include "vector.h"

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

const char* kitoa(int i) {
    char* buf = kmalloc(12); // known to be big enough
    snprintf(buf, 12, "%d", i);
    return buf;
}

char* kstrclone(const char* s) {
    size_t len = strlen(s)+1;
    char* tmp = kmalloc(len);
    strlcpy(tmp, s, len);
    return tmp;
}

int kgetc(void)
{
    char ch = *((volatile unsigned char *) PUTCHAR_ADDRESS) ;
	return ch;
}

void khalt(void) 
{
	*((volatile unsigned char *) HALT_ADDRESS) = 0;
	__builtin_unreachable();
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

#include "user_syscalls.h"

int printf(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, false);
    va_end(va);

//    kputs(buf);

    int rid = message(buf, strlen(buf)+1);
    sink(rid, lookup("~stdio"));

    return 0;
}


int println(const char* fmt, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, fmt);
    vprintf(buf, 1024, fmt, va, true);
    va_end(va);

  //  kputs(buf);
    int rid = message(buf, strlen(buf)+1);
    sink(rid, lookup("~stdio"));


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

int strrcpy(char* buf, size_t bufpos, size_t size, const char* s) {
  size_t newpos = bufpos;
  if (!s) {
      newpos += strlcpy(buf + bufpos, "<NULL>", strlen("<NULL>")+1);
      newpos--;
      return newpos;
  }
  size_t chars = strlcpy(buf + bufpos, s, size - bufpos);
  newpos += chars -1; // remove null
  return newpos;
}

#define PRINT(x) bufpos = strrcpy(buf, bufpos, size, (x))
#define PRINTC(c) buf[bufpos++] = (c);

const char* ko_str(ko* o) {
  char buf[512];
  size_t bufpos = 0;
  size_t size = 512;
  if (!o) {
    return kstrclone("<0,0>");
  }

  PRINTC('<');

  const int BUFSIZE = 64;
  char tmpbuf[BUFSIZE]; // PLENTY of room for scratch stuff
  //PRINT(MAGENTA);
  utoa(tmpbuf, BUFSIZE, ID(o));
  PRINT(tmpbuf);
  //PRINT(WHITE);
  PRINTC(',');
  PRINTC((char)o->type);
  PRINTC('>');
  buf[bufpos] = '\0';

  return kstrclone(buf);
}

int vprintf(char* buf, size_t size, const char* fmt, va_list va, int newline)
{
    const int BUFSIZE = 64;
    char tmpbuf[BUFSIZE]; // PLENTY of room for scratch stuff
    int d;
    unsigned u;
    char *s;
    const char * cs;
    vector* v;
    ko* o;

    size_t fmtpos, bufpos;
    for (fmtpos = 0, bufpos = 0; bufpos < size && fmt[fmtpos] != '\0'; fmtpos++) {
        if (fmt[fmtpos] != '%') {
            buf[bufpos] = fmt[fmtpos];
            bufpos++;
        } else {
            fmtpos++;
            switch (fmt[fmtpos]) {
                case 'k':
                    o = va_arg(va, ko*);
                    cs = ko_str(o); 
                    PRINT(cs);
                    kfree((void*)cs);
                    break;
                case 'd':
                    d = va_arg(va, int);
                    itoa(tmpbuf, BUFSIZE, d);
                    PRINT(tmpbuf);
                    break;
                case 'u':
                    u = va_arg(va, unsigned);
                    utoa(tmpbuf, BUFSIZE, u);
                    PRINT(tmpbuf);
                    break;
                case 'p': // we assume sizeof(void*) = sizeof(unsigned)
                case 'x':
                    u = va_arg(va, unsigned);
                    utoa16(tmpbuf, BUFSIZE, u);
                    PRINT(tmpbuf);
                    break;
                case 's':
                    s = va_arg(va, char*);
                    PRINT(s);
                    break;
                case 'c':
                    d = va_arg(va, int);
                    PRINTC((char)d);
                    break;
                case 'v':
                    v = va_arg(va, vector*);
                    if (v->size == 0) {
                      PRINT("[]");
                      break;
                    }
                    PRINTC('[');
                    cs = vector_join(v, ", ");
                    PRINT(cs);
                    kfree((char*)cs);
                    PRINTC(']');
                    break;
                case 'K':
                    o = va_arg(va, ko*);
                    cs = VIEW(o);
                    PRINT(cs);
                    kfree((void*)cs);
                    break;
                case '%':
                    PRINTC('%');
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
