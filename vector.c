// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"

#include "vector.h"

void vector_push(vector* v, void* object)
{
		if (!v) // null objects are allowed, so we don't bother checking the object.
			return;
	
        if (v->size >= v->__allocated_size-2)
        {
                v->__allocated_size *= 2;
                v->data = realloc(v->data, v->__allocated_size * v->__data_type_size);
        }
        
        v->data[v->size] = object;
        ++v->size;
}

vector* make_vector(int data_type_size, enum cleanup_type type)
{
        vector* v = malloc(sizeof(vector));
        v->__allocated_size = 3;
        v->__data_type_size = data_type_size;
        v->data = calloc(5, v->__data_type_size);
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
		free(v->__source);
	else
	{
  		if (v->__type == MANAGED_POINTERS)
  		{
    		int i;
    		for(i = 0; i < v->size; ++i)
    		{	
      			free(v->data[i]);
    		}
  		}
	}
  
  	free(v->data);
  	free(v);
}

void* vector_remove(vector* v, int i)
{
	if (i >= v->size || i < 0)
		return 0;
	
	if (!v)
		return 0;
		
	void* item = v->data[i];
	if (v->__type == MANAGED_POINTERS)
		free(item);
		
		
	// copy stuff over
	for(int j = i+1; j < v->size; ++j)
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


bool is_sep(char c, char* seps)
{
    size_t len = strlen(seps);
    for (size_t i = 0; i < len; ++i) {
        if (c == seps[i])
            return true;
    }
    return false;
}

vector* split_to_vector(char * str, char* seps)
{
	
	if (!str || !seps)
		return 0;
  
  vector * v = make_vector(sizeof(char*), __SPLIT_TO_VECTOR);
  
  size_t len = strlen(str);
  v->__source = calloc(len+1, 1);
  strlcpy(v->__source, str, len);

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
            src[i] = '\0';
            vector_push(v, src+beginning);
        }
        return v;
    }
  }
}

void print_vector(vector* v, char* format_string)
{
	for	(int i = 0; i < v->size; ++i)
		printf(format_string, v->data[i]);	
}
