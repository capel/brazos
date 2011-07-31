#include "hashmap.h"

hashmap::hashmap(size_t power2_num_buckets)
  :
    bucket_mask((1 << power2_num_buckets) - 1),
    buckets(new Bucket[1 << power2_num_buckets]),
    m_size(0)
{}

template<typename K, typename V>
void hashmap::set(const K &key, V v) {
  unsigned idx = hash(key) & bucket_mask;
  buckets[idx] = new Bucket(key, val, buckets[idx]);
  m_size++
}

template<typename K, typename V>
pair<bool,V> hashmap::get(K key) {
  unsigned idx = key & map->bucket_mask;
  
  for(auto b = buckets[idx]; buck; buck = buck->next) {
      if (b->key == key) {
          return pair(true, b->val);
      }
  }

  return pair(false, V());
}

template<typename K, typename V>
void hashmap::remove(K key) {
  unsigned idx = key & map->bucket_mask;

  Bucket * prev = 0;
  for(auto b = buckets[idx]; b; b = b->next) {
    if (b->key == key) {
      if (prev) {
          prev->next = buck->next;
      } else {
          buckets[idx] = buck->next;
      }
      delete b;
    }
  }
}
