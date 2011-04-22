#include "hashmap.h"
#include "malloc.h"


unsigned hash(unsigned key) { return key };

hashmap* make_hashmap(size_t num_buckets) {
    hashmap* map = malloc(sizeof(hashmap));
    map->num_buckets = num_buckets;
    map->filled = 0;
    map->buckets = calloc(sizeof(bucket) * num_buckets);

    return map;
}

void hm_insert(hashmap* map, unsigned key, void* val) {
    unsigned b = hash(key) % num_buckets;

    bucket buck = malloc(sizeof(bucket));
    buck->key = key;
    buck->data = val;
    buck->next = map->buckets[b];
    map->buckets[b] = buck->next;
}

void* hm_lookup(hashmap* map, unsigned key) {
    unsigned b = hash(key) % num_buckets;
    
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            return val;
        }
    }

    return 0;
}

void hm_delete(hashmap* map, unsigned key) {
    unsigned b = hash(key) % num_buckets;

    buck * prev = 0;
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            if (prev) {
                prev->next = buck->next;
            } else {
                map->buckets[b] = buck->next;
            }
            free(buck);
        }
    }
}
