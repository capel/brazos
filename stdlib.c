#include "stdlib.h"
#include "user_syscalls.h"
#include "stdio.h"

char hexchars[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f' };

int char_to_digit(char c) {
    if (c < 0x30 || c > 0x39)
        return -1;

    return c - 0x30;
}

void memset(char* start, char fill, size_t bytes)
{
    for( ; bytes ; bytes--) {
        *start++ = fill;
    }
}

void memcpy(char* dst,const char * src, size_t len)
{
    if (dst == src)
        return;
    for ( ; len; len--) {
        *dst++ = *src++;
    }
}

int memcmp(const char* a, const char* b, size_t bytes) 
{
    byte r;
    for (size_t i = 0; i < bytes; i++) {
        r = (byte) *a++ - (byte) *b++;
        if (r)
            return r;
    }
    return 0;
}


bool isspace(char c)
{
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
            return true;
        default:
            return false;
    }
}

bool isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

bool isalpha(char c)
{
    return isupper(c) || islower(c);
}

bool isupper(char c)
{
    return (c >= 'A' && c <= 'Z');
}

bool islower(char c)
{
    return (c >= 'a' && c <= 'z');
}

size_t strlen(const char* str) 
{
    size_t r = 0;
    while(*str++) r++;

    return r;
}

size_t strlcpy(char* dst, const char* src, size_t len)
{
    if (len == 0) 
        return -1;

    for(size_t i = 1 ; i < len; i++) {
        *dst++ = *src++;
        if (*(dst-1) == '\0')
            return i;
    }

    *dst = '\0';
    return len;
}

int strcmp(const char* a, const char* b)
{
    int r;
    for (;;) {
        r = (byte) *a - (byte)*b;
        if (r)
            return r;
        if (*a == '\0') // clearly b is also nul
            return 0;
        a++;
        b++;
    }
    return 0;
}

int strncmp(const char* a, const char* b, size_t len)
{
    int r;
    for (; len; --len) {
        r = (byte) *a - (byte)*b;
        if (r)
            return r;
        if (*a == '\0') // clearly b is also nul
            return 0;
        a++;
        b++;
    }
    return 0;

}

/*
char* kstrgcat(const char* a, const char* b, size_t alen, size_t blen)
{
    if (alen == 0 || blen == 0)
        return NULL;

    char* buf = malloc(alen + blen - 1);

    size_t len = strlcpy(buf, a, alen);
    buf += len - 1; // remove the NULL strcpy appends

    size_t len = strlcpy(buf, b, blen);

    return buf;
}
*/

void reverse(char* s)
{
    int i, j;
    char c;
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}

void utoa(char* buf, size_t size, unsigned u)
{
    size_t i = 0;
    do {
        buf[i++] = u % 10  + '0';
    } while (((u /= 10) > 0) && i < size);
    buf[i] = '\0';    

    reverse(buf);
}

void itoa(char* buf, size_t size, int i)
{
    if (i < 0) {
        buf[0] = '-';
        utoa(buf+1, size-1, (unsigned) -i);
    } else {
        utoa(buf, size, (unsigned) i);
    }
}

void utoa16(char* buf, size_t size, unsigned x)
{
    if (size < 11) {
        buf[0] = '\0';
        return;
    }
    
    buf[0] = '0';
    buf[1] = 'x';
    int offset, pos, leading;
    leading = true;
    for (pos = 0, offset = 28; offset > 0; offset -= 4) {
        char c = hexchars[(x >> offset) & 0xF];
        if (c == '0' && leading)
            continue;
        leading = false;
        buf[pos++] = c;
    }
    buf[pos++] = hexchars[x & 0xF]; // we always want at least 1 zero
    buf[pos] = '\0';

}

bool isdigits(const char* c) {
    if (*c != '-' && !isdigit(*c))
        return false;

    c++;

    for(;;) {
        if (!*c)
            return true;
        if (!isdigit(*c))
            return false;
        c++;
    }
}

int power(int a, int b) {
    int res = 1;
    for(; b > 0; b--) {
        res *= a;
    }
    return res;
}

int atoi(const char* buf) {
    if (!isdigits(buf))
        return 0;
    
    size_t len = strlen(buf);
    bool negative = false;
    int i = 0;
    
    if (buf[0] == '-') {
        negative = true;
        buf++;
        len--;
    }

    size_t pow = 0;
    for(int pos = len-1; pos >= 0; pos--, pow++) {
        int digit = char_to_digit(buf[pos]);
        i += digit * power(10, pow);
    }

    if (negative)
        i = -i;
    
    return i;
}

bool lock(lock_t* l)
{
    while(*l == 1) {
        yield();
    }
    *l = 1;
    return true;
}

bool unlock(lock_t* l)
{
    if (*l == 0) {
        debug("Attempting to unlock an unlocked lock.");
    }
    *l = 0;
    return true;
}
