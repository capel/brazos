#ifndef KIO_H
#define KIO_H

#ifdef USER
#error __file__ " included from user code."
#endif

char kgetch();
void khalt() __attribute__((noreturn));

void kputs(const char *s);
int snprintf(char* buf, size_t size, const char* fmt, ...);

int kprintf(const char* fmt, ...);
void _printk(const char* fmt, ...);
void _panic(const char* fmt, ...) __attribute__((noreturn));


#define BLACK "\033[22;30m"
#define RED "\033[22;31m"
#define GREEN "\033[22;32m"
#define BROWN "\033[22;33m" 
#define BLUE "\033[22;34m"
#define MAGENTA "\033[22;35m" 
#define CYAN "\033[22;36m"
#define GRAY "\033[22;37m"
#define DARK_GRAY "\033[01;30m"
#define LIGHT_RED "\033[01;31m"
#define LIGHT_GREEN "\033[01;32m" 
#define YELLOW "\033[01;33m"
#define LIGHT_BLUE "\033[01;34m"
#define LIGHT_MAGENTA "\033[01;35m" 
#define LIGHT_CYAN "\033[01;36m"
#define WHITE "\033[01;37m" 

#define panic(x, args...) _panic(RED "PANIC " WHITE  __FILE__ ":%d [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#define printk(x, args...) _printk(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)

#endif
