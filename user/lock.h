#include "syscalls.h"
#include "stdio.h"

inline static bool lock(lock_t* l)
{
    while(*l == 1) {
        yield();
    }
    *l = 1;
    return true;
}

inline static bool unlock(lock_t* l)
{
    if (*l == 0) {
        debug("Attempting to unlock an unlocked lock.");
    }
    *l = 0;
    return true;
}
