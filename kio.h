#ifndef KIO_H
#define KIO_H


char kgetch();
void halt(void) ;

void kputs(const char *s);
int snprintf(char* buf, size_t size, const char* fmt, ...);

int kprintf(const char* fmt, ...);
int printf(const char* fmt, ...);
#endif
