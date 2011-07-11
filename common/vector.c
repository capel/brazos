// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include "stdlib.h"

#include "vector.h"

void vector_push(vector* v, void* object)
{
		if (!v) // null objects are allowed, so we don't bother checking the object.
			return;
	
        if (v->size >= v->__allocated_size-2)
        {
                v->__allocated_size *= 2;
                v->data = v->__alloc_funcs->realloc(v->data, v->__allocated_size * v->__data_type_size);
        }
        
        v->data[v->size] = object;
        ++v->size;
}

vector* _make_vector(int data_type_size, enum cleanup_type type, const alloc_funcs* funcs)
{
        vector* v = funcs->malloc(sizeof(vector));
        v->__alloc_funcs = funcs;
        v->__allocated_size = 3;
        v->__data_type_size = data_type_size;
        v->data = v->__alloc_funcs->calloc(3, v->__data_type_size);
        v->size = 0;
		v->__source = 0;
		v->__type = type;

        return v;
}

void cleanup_vector(vector* v)
{
	if (!v)
		return;

	if (v->__type == __SPLIT_TO_VECTOR) 
	    v->__alloc_funcs->free(v->__source);
	else if (v->__type == MANAGED_POINTERS) {
        size_t i;
        for(i = 0; i < v->size; ++i)
        {	
            v->__alloc_funcs->free(v->data[i]);
        }
	}
  
  	v->__alloc_funcs->free(v->data);
  	v->__alloc_funcs->free(v);
}

void* vector_remove(vector* v, size_t i)
{
	if (i >= v->size)
		return 0;
	
	if (!v)
		return 0;
		
	void* item = v->data[i];
	if (v->__type == MANAGED_POINTERS)
		v->__alloc_funcs->free(item);
		
		
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




vector* _split_to_vector(const char * str, const char* seps, const alloc_funcs* funcs)
{
	
	if (!str || !seps)
		return 0;
  
  vector * v = _make_vector(sizeof(char*), __SPLIT_TO_VECTOR, funcs);
 

  size_t len = strlen(str);
  v->__source = v->__alloc_funcs->calloc(len+1, 1);
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

      if (i >= len)
        return v;

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
        return v;
    }
  }
}

void print_vector(vector* v, const char* format_string, size_t start)
{
	for	(size_t i = start; i < v->size; ++i)
		v->__alloc_funcs->printf(format_string, v->data[i]);	
}

void* vector_best(vector* v, vector_better_func better) {
  if (v->size == 0) {
    return NULL;
  }
  void* best;

  for(size_t i = 0; i < v->size; i++) {
    if (!best) best = v->data[i];
    if (better(v->data[i], best)) best = v->data[i];
  }
  return best;
}

