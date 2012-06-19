#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <file.h>

int Printf(const char* fmt, ...)
{
    char buf[4096];

    va_list va;
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);

    int r = _write(1, buf, strlen(buf));
    assert(r > 0);

    int s = _sync(1);
    assert(s == 0);
    return r;
}

char* strclone(const char* s) {
    size_t len = strlen(s)+1;
    char* tmp = malloc(len);
    strcpy(tmp, s);
    return tmp;
}

void rtrim(char* s) {
  for(int i = strlen(s) - 1; i >= 0; i--) {
    if (s[i] != ' ') return;
    s[i] = '\0';
  }
}
