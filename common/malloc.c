#include "malloc.h"
#include "stdlib.h"

#define CAPEL_MAGIC_NUMBER 0xdeadbeef

#define NULL 0

void exit();

typedef struct allocated_node_t
{
	unsigned magic_number;
	int size;
} allocated_node;


typedef struct free_node_t
{
	int free;	
	struct free_node_t * next;
} free_node;


void* allocate_free_space(free_node * node, int size, malloc_data* md);
free_node* make_free_node(void* address, int size, malloc_data* md);
free_node* free_allocated_space(allocated_node* node, malloc_data* md);
void merge_adjacent_space(free_node* node, malloc_data* md);

free_node* merge_nodes(free_node* a, free_node* b, malloc_data* md);

free_node* find_fit(int size, malloc_data* md);

void list_insert(free_node* node, malloc_data* md);
void list_remove(free_node* node, malloc_data *md);

#define PAGE_SIZE 4096


void get_space(size_t pages, malloc_data * md) {
    void * start = md->get_pages(pages);

	free_node* node = make_free_node(start, md->size, md);
	node->next = md->head;
	md->head = node;
//	db(" Head: %p", md->head);
	

//	db("Sizeof(free_node): %d Sizeof(allocated_node): %d", sizeof(free_node), sizeof(allocated_node));
}

int _mem_init(size_t pages, malloc_data * md)
{
	
	md->size = pages * PAGE_SIZE;
    get_space(pages, md);
    return 0;
}




void* _malloc(size_t size, malloc_data * md)
{
	if (size <= 0)
		return NULL;


	size += sizeof(allocated_node); // we need to store our info there, too
	if (size % 4 != 0)
		size += 4 - (size % 4);
		
	assert(size % 4 == 0); // the pointer must be 4-byte aligned
	
	size = MAX(size, sizeof(free_node));
	

	free_node* node = find_fit(size,md);
	

	if (!node)
	{
		md->db("No available space found for size %d. Getting more memory from the kernel", size);
		get_space(md->size / PAGE_SIZE, md); // get as much new as we had before.
        
        // try again
        node = find_fit(size, md);
		
		// it failed again, give up
		if (!node)
		{
		    md->db("No available space _again_. Giving up");
		    exit();
		    return NULL;
		}
	}
	
	void* p = allocate_free_space(node, size,md);
	
   // md->db("Alloc of %d at %p", size, p);

//    Mem_Dump(md);
	
	assert(p);
	return p;
}

void* _realloc(void* ptr, size_t size, malloc_data * md) 
{
//	db("Realloc of size %d" , size);
	allocated_node* a = ptr - sizeof(allocated_node);
	
	if (a->magic_number != CAPEL_MAGIC_NUMBER)
	{
			md->db("Bad pointer %p passed to Mem_Free.", ptr);
			exit();
			return NULL;	
	}
    
    size_t old_size = a->size;

    if (old_size >= size)
    {
       // md->db("Attempting to realloc to a smaller space %d -> %d. Wtf?", old_size, size);
        return ptr;
    }

//    db("Old size: %d", old_size);
    void* new_space = _malloc(size,md);
    memcpy(new_space, ptr, old_size);
    _free(ptr, md);


    return new_space;
}

void* _calloc(size_t size, size_t obj_size, malloc_data * md)
{
    size_t real_size = size*obj_size;
//	db("Calloc of size %d" , real_size);

    void* ptr = _malloc(real_size,md);
    memset(ptr, 0, real_size);
    return ptr;
}


int _free(void *ptr, malloc_data * md)
{
 //   db("Start free");
	if (!ptr)
	{
		return -1;
	}
	
	allocated_node* a = ptr - sizeof(allocated_node);
	if (a->magic_number != CAPEL_MAGIC_NUMBER)
	{
			md->db("Bad pointer %p passed to Mem_Free.", ptr);
			return -1;	
	}

//	Mem_Dump(md);

	free_node* node = free_allocated_space(a,md);
	merge_adjacent_space(node,md);
	
//	db("Post free");
  //  Mem_Dump(md);

//	md->db("Free at address %p", ptr);
	
	return 0;
}
void Mem_Dump(malloc_data* md)
{
	md->db("=======FREE MEMORY DUMP=====");
	md->db("Total size : %d", md->size);
	md->db("Address    : Free");
	int i =0; 
	free_node* current = md->head;
	for(; current; current = current->next)
	{
		i += current->free;
		md->db("%p : %d ", current, current->free);
	}
	md->db("Allocated: %d", md->size - i);
	md->db("============================");
}



free_node* make_free_node(void* address, int size, malloc_data *md)
{
//	db("addr: %p, size: %d", address, size); 
	//assert(address < _start + md->size);
	free_node* node = address;
	node->next = NULL;
	node->free = size;
	
	return node;
}

free_node* free_allocated_space(allocated_node* node, malloc_data * md)
{
	free_node* new_node = make_free_node(node, node->size,md);
	list_insert(new_node,md);
	return new_node;
}

void* allocate_free_space(free_node * node, int required, malloc_data*md)
{	
//	db("Node->free: %d, Required: %d, diff: %d", node->free, required, node->free - required);
	assert(node->free >= required);

	void* node_addr = (void*) node;
	node_addr += required;
	
//	db("Pointers: %p - %p = %d ?= %d", node_addr, node, node_addr - (void*)node, required); 

	if (node->free - required > 0) // if we have space left over
	{
		free_node* new_node = make_free_node(node_addr, node->free - required,md);
		list_insert(new_node,md);
	}
	
	list_remove(node,md);
	
	// node is now out of the list and we can replace it
	allocated_node* a = (void*)node;
	a->magic_number = CAPEL_MAGIC_NUMBER;
	a->size = required;
	
	return (void*)a + sizeof(allocated_node); // point them to the space, not the node
}

void list_remove(free_node* node, malloc_data * md)
{
	assert(node);
	assert(md->head);
	//db("remove node: %p", node); 
	
	if (node == md->head)
	{
		md->head = node->next;
		return;
	}
	
	free_node* current = md->head;
	
	while(current->next != node)
	{
		current = current->next;	
	}
	
	current->next = node->next;
}

void list_insert(free_node* node, malloc_data* md)
{
	assert(node);
	//db("insert node: %p", node); 
	
	node->next = NULL;
	
    node->next = md->head;
    md->head = node;
    return;
	
}

free_node* find_fit(int required, malloc_data* md)
{
//	assert(required >= 16);

    free_node *current = md->head;

    for(; current != NULL; current = current->next) {
        if (current->free >= required) {
            return current;	
        }
    }
    // if we reach the end of the list with first or best, we FAIL.
    return 0;
}

// this function oddly has two repeated sections of code
// this is because if we are looking to merge nodes, a node can be merged twice:
// once before, and once behind
// so the merge algorithm needs to be run twice
//
// a preassumption of this function is that the free_node list is fully merged already 
// (sans the new node)
void merge_adjacent_space(free_node* node, malloc_data * md)
{
	assert(node);
	assert(md->head);
	
	free_node* current = md->head;
	void* temp;
	for(; current; current = current->next) {
		if (current == node) {
			continue;
		} else if (current < node) {
			temp = (void*) current;
			if (temp + current->free == node) {
				node = merge_nodes(current, node,md);
				break;	
			}	
		} else {
			temp = (void*) node;
			if (temp + node->free == current) {
				node = merge_nodes(node, current,md);
				break;
			}
		}
	}
	
	current = md->head;
	for(; current; current = current->next) {
		if (current == node) {
			continue;
		} else if (current < node) {
			temp = (void*) current;
			if (temp + current->free == node) {
				node = merge_nodes(current, node,md);
				break;	
			}	
		} else {
			temp = (void*) node;
			if (temp + node->free == current) {
				node = merge_nodes(node, current,md);
				break;
			}
		}
	}
}

free_node* merge_nodes(free_node* a, free_node* b, malloc_data * md)
{
//	db("Merge: %p, %p", a, b); 
	// a must be < b
	if (a > b)
	{
		free_node * temp = b;
		b = a;
		a = temp;	
	}
	
	list_remove(b,md);
	list_remove(a,md);
	
	a->free += b->free;
	a->next = NULL;
	
	b->free = 4345345;
	b->next = (void*)0xdeadbeef; // just to make sure
	
	list_insert(a,md);
	return a; // b in INVALID now
}