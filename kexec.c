#include "kexec.h"
#include "types.h"
#include "stdlib.h"

extern void sh_main(void);
extern void bc_main(void);

char* names[] = {
    "sh",
    "bc"
};

void* addrs[] = {
    sh_main,
    bc_main
};

void* program_lookup(char* name) {
    for(size_t i = 0; i < sizeof(names); i++) {
        if(0 == strcmp(name, names[i])) {
            return addrs[i];
        }
    }
    return 0;
}
