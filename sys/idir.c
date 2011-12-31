#include "idir.h"
#include "../mem.h"
#include "../kvector.h"

typedef struct _bucket {
    unsigned key;
    ko * value;
    struct _bucket *next;
} bucket;

static void add_keys(vector* v, bucket* b) {
  if (!b) return;
  
  vector_push(v, (char*)kitoa(b->key));
  add_keys(v, b->next);
}

vector* idir_keys(idir* map) {
  vector* v = kmake_vector(sizeof(unsigned), MANAGED_POINTERS);
  for(size_t i = 0; i < map->size; i++) {
    add_keys(v, map->buckets[i]);
  }
  return v;
}


bool idir_insert(idir* map, unsigned key , ko* value) {
    if (idir_lookup(map, key)) {
        return false;
    }
    
    unsigned b = key & map->bucket_mask;

    bucket * buck = kmalloc(sizeof(bucket));
    buck->key = key;
    buck->value = value;
    kget(value);
    buck->next = map->buckets[b];
    map->buckets[b] = buck;

    return true;
}

ko* idir_lookup(idir* map, unsigned key) {
    unsigned b = key & map->bucket_mask;
    
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) return buck->value;
    }

    return 0;
}

static void free_bucket(bucket* b, bool follow) {
  if (!b) return;

  kput(b->value);
  bucket *next = b->next;
  kfree(b);

  if (follow) free_bucket(next, true);
}

bool idir_delete(idir* map, unsigned key) {
    unsigned b = key & map->bucket_mask;

    bucket * prev = 0;
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            if (prev) {
                prev->next = buck->next;
            } else {
                map->buckets[b] = buck->next;
            }
            free_bucket(buck, false);
            return true;
        }
    }
    return false;
}


void idir_cleanup(ko* o) {
  idir* map = (idir*)o;
  for (size_t i = 0; i < map->size; i++) {
    free_bucket(map->buckets[i], true);
  }
}

static ko* idir_ext_lookup(dir* d, const char* name) {
  return idir_lookup((idir*)d, atoi(name));
}

static err_t idir_ext_link(dir* d, ko* child, const char* name) {
  return idir_insert((idir*)d, atoi(name), child) ? 0 : E_ERROR;
}

static err_t idir_ext_unlink(dir* d, const char* name) {
  return idir_delete((idir*)d, atoi(name)) ? 0 : E_NOT_FOUND;
}

static dir_vtable idir_vt = {
  .lookup = idir_ext_lookup,
  .link = idir_ext_link,
  .unlink = idir_ext_unlink,
};

#define PRINT(x) bufpos = strrcpy(buf, bufpos, size, (x))
#define PRINTC(c) buf[bufpos++] = (c);

static const char * idir_view(ko* dmap) {
  char buf[4096];
  size_t bufpos = 0;
  size_t size = 4096;
  idir* h = (idir*)dmap;
  PRINTC('{');

  vector* keys = idir_keys(h);
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

idir* mk_idir() {
  size_t power2_num_buckets = 7; // larger than normal dir
  idir* map = (idir*)mk_ko(sizeof(idir), idir_cleanup, idir_view, KO_DIR);
  map->bucket_mask = (1 << power2_num_buckets) - 1;
  map->size = (1 << power2_num_buckets);
  map->buckets = kcalloc(sizeof(bucket), map->size);
  printk("mask %d size %d buckest %d", map->bucket_mask, map->size, map->buckets);


  DIR(map)->v = &idir_vt;

  return map;
}
