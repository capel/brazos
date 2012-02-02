#include "kio.h"


#define CONSUME(c) do { if(s[*pos] != c) { DIE(); } (*pos)++; } while (0)
#define NOM() s[(*pos)++]

#define NOM_SPACE() while (isspace(s[*pos])) { (*pos)++; }

#define PARSE(name) variant name(const char * s, size_t *pos, bool* die)

PARSE(_parse);

#define DIE() do { *die = true; printk("Parse error"); return Null(); } while(0)

#define HOPE(cond) if (!(cond)) { DIE(); }
