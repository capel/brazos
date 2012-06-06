#include <stdlib.h>
#include <string.h>

char* strclone(const char* s) {
    size_t len = strlen(s)+1;
    char* tmp = malloc(len);
    strcpy(tmp, s);
    return tmp;
}
