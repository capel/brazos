#include "khashmap.h"
#include "../mem.h"
#include "../kvector.h"

static unsigned hash(const char* s) {
  unsigned h = 0;
  unsigned m = 1;
  for(int i = 0; s[i]; i++) {
    h += m++ * (unsigned char)s[i];
  }
  return h;
}

khashmap* mk_khashmap(size_t power2_num_buckets) {
  assert(power2_num_buckets <= 31);

  khashmap* map = (khashmap*) kmalloc(sizeof(khashmap));
  map->bucket_mask = (1 << power2_num_buckets) - 1;
  map->size = (1 << power2_num_buckets);
  map->buckets = (kbucket**) kcalloc(sizeof(kbucket), map->size);

  return map;
}

static void add_keys(vector* v, kbucket* b) {
  if (!b) return;
  
  vector_push(v, (char*)b->key);
  add_keys(v, b->next);
}

vector* khm_keys(khashmap* map) {
  vector* v = kmake_vector(sizeof(char*), UNMANAGED_POINTERS);
  for(size_t i = 0; i < map->size; i++) {
    add_keys(v, map->buckets[i]);
  }
  return v;
}


bool khm_insert(khashmap* map, const char* key, ko* value) {
    if (khm_lookup(map, key)) {
        printk("hm_insert: Double insert on key %u\n");
        return false;
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

ko* khm_lookup(khashmap* map, const char* key) {
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

bool khm_delete(khashmap* map, const char* key) {
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
            return true;
        }
    }
    return false;
}


void khm_cleanup(khashmap* map) {
  for (size_t i = 0; i < map->size; i++) {
    free_bucket(map->buckets[i], true);
  }
}


