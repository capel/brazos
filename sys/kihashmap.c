#include "kihashmap.h"
#include "../mem.h"
#include "../kvector.h"

typedef struct _bucket {
    unsigned key;
    ko * value;
    struct _bucket *next;
} bucket;

kihashmap* mk_kihashmap(size_t power2_num_buckets) {
  assert(power2_num_buckets <= 31);

  kihashmap* map = kmalloc(sizeof(kihashmap));
  map->bucket_mask = (1 << power2_num_buckets) - 1;
  map->size = (1 << power2_num_buckets);
  map->buckets = kcalloc(sizeof(bucket), map->size);

  return map;
}

static void add_keys(vector* v, bucket* b) {
  if (!b) return;
  
  vector_push(v, (void*)b->key);
  add_keys(v, b->next);
}

vector* kihm_keys(kihashmap* map) {
  vector* v = kmake_vector(sizeof(unsigned), UNMANAGED_POINTERS);
  for(size_t i = 0; i < map->size; i++) {
    add_keys(v, map->buckets[i]);
  }
  return v;
}


bool kihm_insert(kihashmap* map, unsigned key , ko* value) {
    if (kihm_lookup(map, key)) {
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

ko* kihm_lookup(kihashmap* map, unsigned key) {
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

bool kihm_delete(kihashmap* map, unsigned key) {
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


void kihm_cleanup(kihashmap* map) {
  for (size_t i = 0; i < map->size; i++) {
    free_bucket(map->buckets[i], true);
  }
}


// Returns the lowest key in the map
unsigned kihm_lowest(kihashmap* map) {
  if (map->size == 0) return 0;
  vector* v = kihm_keys(map);
  unsigned key = (unsigned)v->data[0];
  unsigned key2;
  for(size_t i = 0; i < v->size; i++) {
    key2 = (unsigned)v->data[0];
    if (key2 < key) key = key2;
  }
  return key;
}

// Returns the highest key in the map
unsigned kihm_highest(kihashmap* map) {
  if (map->size == 0) return 0;
  vector* v = kihm_keys(map);
  unsigned key = (unsigned)v->data[0];
  unsigned key2;
  for(size_t i = 0; i < v->size; i++) {
    key2 = (unsigned)v->data[0];
    if (key2 > key) key = key2;
  }
  return key;
}
