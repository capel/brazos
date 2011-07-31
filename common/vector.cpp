// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include "stdlib.h"

#include "vector.h"

template<typename T>
void vector::push(T t) {
  if (size() >= m_allocated-2)
  {
    m_allocated *= 2;
    m_data = new T[allocated];
  }
  
  m_data[m_size++] = t;
}

template<typename T>
T vector::pop() {
  T t = m_data[size()];
  remove(size());
  return t;
}

template<typename T>
vector::vector(size_t size)
  : m_allocated(3), m_size(0), m_data(new T[m_allocated])
{}

vector::~vector() {
  delete [] m_data;
}

void vector::remove(size_t idx)
{
	if (idx >= size())
		return;
		
	// copy stuff over
	for(size_t j = i+1; j < v->size; ++j)
	{
		m_data[j-1] = m_data[j];	
	}
	
  --size;
}

/*
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

void* vector_best(vector* v, vector_better_func better) {
  if (v->size == 0) {
    return NULL;
  }
  void* best = NULL;

  for(size_t i = 0; i < v->size; i++) {
    if (!best) best = v->data[i];
    if (better(v->data[i], best)) best = v->data[i];
  }
  return best;
}
*/
