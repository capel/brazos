#include "kio.h"
#include "mem.h"
#include "../../common/stdlib.h"
#include "../../common/vector.h"
#include "ent.h"

#include "proc.h"
#include "sched.h"


typedef ent string;
string * mkstr(const char *cstring);

typedef ent integer;
integer * mkint(int i);

typedef ent sched;
sched * mkschd(void);

typedef ent file;
file* mkfile(void);

typedef ent dir;
dir* mkdir(void);

void mkroot(void);
kdir* root(void);

#define PAGE_SIZE 4096

typedef ent pages;
typedef ent mm;

mm* kcreate_mm(void);
void ksetup_memory(void);

typedef ent program;
program mkprogram();

void* kmalloc(size_t size);
void* kcalloc(size_t size, size_t obj_size);
void* krealloc(void *ptr, size_t newsize);
int kfree(const void *ptr);

extern alloc_funcs kernel_alloc_funcs;

static inline vector* kmake_vector(int data_type_size, enum cleanup_type type) {
    return _make_vector(data_type_size, type, &kernel_alloc_funcs);
}
static inline vector* ksplit_to_vector(const char * str, const char* seps) {
    return _split_to_vector(str, seps, &kernel_alloc_funcs);
}

