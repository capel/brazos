// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

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
	int j;
	for(j = i+1; j < v->size; ++j)
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


vector* split_to_vector(char * str, char* seps)
{
	
	if (!str || !seps)
		return 0;
	int i;
	
	
  vector * v = make_vector(sizeof(char*), __SPLIT_TO_VECTOR);
  
  v->__source = calloc(strlen(str)+1, 1);
  strcpy(v->__source, str);
  char * token = strtok(v->__source, seps);
  while (token != 0)
  {
  	int size = strlen(token);
  	for(i=0;i<size;++i)
  	{
  		// only add it if it contains a non-whitespace character.
  		if (!isspace(token[i]))
  		{
    		vector_push(v, token);
    		break;
  		} 	
  	}
    token = strtok(NULL, seps);
  }
  return v;
}

void print_vector(vector* v, char* format_string)
{
	int i;
	for	(i = 0; i < v->size; ++i)
		printf(format_string, v->data[i]);	
}
