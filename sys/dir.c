#include "../kvector.h"
#include "ko.h"

static unsigned hash(const char* s) {
  unsigned h = 0;
  unsigned m = 1;
  for(int i = 0; s[i]; i++) {
    h += m++ * (unsigned char)s[i];
  }
  return h;
}

typedef struct _bucket {
    const char* key;
    ko * value;
    struct _bucket *next;
} kbucket;

typedef struct _hashmap {
    dir d;
    size_t bucket_mask; // the mask to use to find the modulo
    size_t size;
    kbucket** buckets;
} khashmap;

static err_t khm_insert(dir* dmap, ko* value, const char* key);
static ko* khm_lookup(dir* dmap, const char* key);
static err_t khm_delete(dir* dmap, const char* key);

static void add_keys(vector* v, kbucket* b) {
  if (!b) return;
  
  vector_push(v, (char*)b->key);
  add_keys(v, b->next);
}

static vector* khm_keys(khashmap* map) {
  vector* v = kmake_vector(sizeof(char*), UNMANAGED_POINTERS);
  for(size_t i = 0; i < map->size; i++) {
    add_keys(v, map->buckets[i]);
  }
  return v;
}


static err_t khm_insert(dir* dmap, ko* value, const char* key) {
    khashmap* map = (khashmap*)dmap;
    if (khm_lookup(dmap, key)) {
      khm_delete(dmap, key);
    }
    
    unsigned b = hash(key) & map->bucket_mask;

    kbucket * buck = kmalloc(sizeof(kbucket));
    buck->key = kstrclone(key);
    buck->value = value;
    kget(value);
    buck->next = map->buckets[b];
    map->buckets[b] = buck;

    return true;
}

static ko* khm_lookup(dir* dmap, const char* key) {
    khashmap* map = (khashmap*)dmap;
    unsigned b = hash(key) & map->bucket_mask;
    
    for(kbucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (!strcmp(buck->key, key)) return buck->value;
    }

    return 0;
}

static void free_bucket(kbucket* bucket, bool follow) {
  if (!bucket) return;

  kput(bucket->value);
  kfree((void*)bucket->key);
  kbucket *next = bucket->next;
  kfree(bucket);

  if (follow) free_bucket(next, true);
}

static err_t khm_delete(dir* dmap, const char* key) {
    khashmap* map = (khashmap*)dmap;
    unsigned b = hash(key) & map->bucket_mask;

    kbucket * prev = 0;
    for(kbucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (!strcmp(buck->key, key)) {
            if (prev) {
                prev->next = buck->next;
            } else {
                map->buckets[b] = buck->next;
            }
            free_bucket(buck, false);
            return 0;
        }
    }
    return E_NOT_FOUND;
}


static void khm_cleanup(ko* kmap) {
  khashmap* map = (khashmap*)kmap;
  for (size_t i = 0; i < map->size; i++) {
    free_bucket(map->buckets[i], true);
  }
  kfree(map->buckets);
}

#define PRINT(x) bufpos = strrcpy(buf, bufpos, size, (x))
#define PRINTC(c) buf[bufpos++] = (c);

static const char * khm_view(ko* dmap) {
  char buf[4096];
  size_t bufpos = 0;
  size_t size = 4096;
  khashmap* h = (khashmap*)dmap;
  PRINTC('{');

  vector* keys = khm_keys(h);
  for(size_t i = 0; i < keys->size; i++) {

    //PRINT(CYAN);
    PRINT(keys->data[i]);
    //PRINT(WHITE);
    PRINTC(':');

    const char * s = ko_str(LOOKUP(DIR(h), keys->data[i]));
    PRINT(s);
    kfree((void*)s);

    if (i != keys->size-1) {
      PRINTC(',');
      //PRINTC(' ');
    }
  }
  PRINTC('}');
  PRINTC('\0');

  cleanup_vector(keys);
  return kstrclone(buf);
}

#undef PRINT
#undef PRINTC

static dir_vtable khm_vt = {
  .lookup = khm_lookup,
  .link = khm_insert,
  .unlink = khm_delete,
};

dir* mk_dir() {
  size_t power2_num_buckets = 3;
  assert(power2_num_buckets <= 31);

  khashmap* map = (khashmap*)mk_ko(sizeof(khashmap), khm_cleanup, khm_view, KO_DIR);
  map->bucket_mask = (1 << power2_num_buckets) - 1;
  map->size = (1 << power2_num_buckets);
  map->buckets = (kbucket**) kcalloc(sizeof(kbucket), map->size);

  DIR(map)->v = &khm_vt;

  return DIR(map);
}
