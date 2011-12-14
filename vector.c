// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include "stdlib.h"
#include "stdio.h"
#include "mem.h"

#include "vector.h"
#include "kio.h"

void vector_push(vector* v, char* object)
{
		if (!v) // null objects are allowed, so we don't bother checking the object.
			return;
	
        if (v->size >= v->__allocated_size-2)
        {
                v->__allocated_size *= 2;
                void* old = v->data;
                v->data = kmalloc(v->__allocated_size * sizeof(char*));
                memcpy((char*)v->data, old, v->__allocated_size * sizeof(char*));
        }
        
        v->data[v->size] = object;
        ++v->size;
}

vector* kmake_vector(int data_type_size, enum cleanup_type type)
{
        vector* v = kmalloc(sizeof(vector));
        v->__allocated_size = 8;
        v->data = malloc(8 * sizeof(char*));
        v->size = 0;
        v->__source = 0;
        v->__type = type;

        return v;
}

void cleanup_vector(vector* v)
{
	if (!v)
		return;

	if (v->__type == __SPLIT_TO_VECTOR) {
	    kfree(v->__source);
	} else if (v->__type == MANAGED_POINTERS) {
      for(size_t i = 0; i < v->size; ++i) {	
         kfree(v->data[i]);
      }
	}
  
  kfree(v->data);
  kfree(v);
}

void* vector_remove(vector* v, size_t i)
{
	if (i >= v->size)
		return 0;
	
	if (!v)
		return 0;
		
	void* item = v->data[i];
	if (v->__type == MANAGED_POINTERS)
		kfree(item);
		
		
	// copy stuff over
	for(size_t j = i+1; j < v->size; ++j)
	{
		v->data[j-1] = v->data[j];	
	}
	
	
	// shorten the vector
	v->data[v->size-1] = 0;
	--v->size;
	
	if (v->__type == UNMANAGED_POINTERS)
		return item;
	else
		return 0;
}


bool is_sep(const char c, const char* seps)
{
    size_t len = strlen(seps);
    for (size_t i = 0; i < len; ++i) {
        if (c == seps[i])
            return true;
    }
    return false;
}

vector* ksplit_to_vector(const char * str, const char* seps)
{
	
	if (!str || !seps)
		return 0;
  
  vector * v = kmake_vector(sizeof(char*), __SPLIT_TO_VECTOR);

  size_t len = strlen(str);
  v->__source = kmalloc(len+1);
  strlcpy(v->__source, str, len+1);

  // for ease of use
  char* src = v->__source;
 
  size_t i = 0;
  size_t beginning;


  while (true) {
      // skip beginning stuff
      for(; i < len; ++i) {
        if (!is_sep(src[i], seps)) {
            break;
        }
      }

      if (i >= len) {
        vector_push(v, NULL);
        return v;
      }

    beginning = i;
    for (; i < len; ++i) {
        if (is_sep(src[i], seps)) {
            src[i++] = '\0';
            vector_push(v, src+beginning);
            break;
        }
    }


    if (i == len) {
        if (i != beginning) {
            src[++i] = '\0';
            vector_push(v, src+beginning);
        }
        vector_push(v, NULL);
        return v;
    }
  }
}

void print_vector(vector* v, const char* format_string, size_t start)
{
	for	(size_t i = start; i < v->size; ++i) {
		kprintf(format_string, v->data[i]);	
	}
	kprintf("\n");
}

const char* vector_join(vector* v, const char* joiner) {
  if (v->size == 0) { return ""; }
  size_t needed = 0;
  size_t pos = 0;
  size_t joiner_len = strlen(joiner);
  bool restore = false;
  if (v->data[v->size-1] == 0) {
    v->size--;
    restore = true;
  }
  for(size_t i = 0; i < v->size; i++) {
    needed += strlen(v->data[i]) + joiner_len;
  }

  char* s = kmalloc(needed + 3);

  for(size_t i = 0; i < v->size; i++) {
    size_t len = strlen(v->data[i]);
    memcpy(s + pos, v->data[i], len);
    pos += len;
    if (i != v->size-1) {
      memcpy(s + pos, joiner, joiner_len);
      pos += joiner_len;
    } else {
      pos += 1;
    }
  }
  s[pos-1] = '\0';
  if (restore)
    v->size++;

  return s;
}
