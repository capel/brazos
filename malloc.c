#include "mem.h"

#define CAPEL_MAGIC_NUMBER 0xdeadbeef

#define MAX(A, B) (A) > (B) ? (A) : (B) 

typedef struct free_node_t
{
	int free;	
	struct free_node_t * next;
} free_node;

typedef struct allocated_node_t
{
	int magic_number;
	int size;
} allocated_node;

void* allocate_free_space(free_node * node, int size);
free_node* make_free_node(void* address, int size);
free_node* free_allocated_space(allocated_node* node);
void merge_adjacent_space(free_node* node);

free_node* merge_nodes(free_node* a, free_node* b);

free_node* find_fit(int size);

void list_insert(free_node* node);
void list_remove(free_node* node);

static free_node* _head;
static int _policy;
static int _size;
static void* _start;

int Mem_Init(int sizeOfRegion, int policy)
{
	static int called = 0;
	if (called == 0)
		called = 1;
	else
		return -1; // this has already been called once.
		
	if (sizeOfRegion <= 0)
		return -1;
		
	if (policy < 0 || policy > 2)
		return -1;
		
	
	_policy = policy;
	
	// round up to page size
	if (sizeOfRegion % getpagesize() != 0)
	{
		sizeOfRegion += getpagesize() - (sizeOfRegion % getpagesize());	
	}
	
	_size = sizeOfRegion;
	
	printf("Init: size: %d, Policy: %d ", _size, _policy);


    // TODO get memory

	_head = make_free_node(_start, _size);
	printf(" Head: %ld\n", (long unsigned int)_head % (2 << 15));
	

	printf("Sizeof(free_node): %ld Sizeof(allocated_node): %ld\n", sizeof(free_node), sizeof(allocated_node));

	return 0;
}


void *Mem_Alloc(int size)
{
	if (size <= 0)
		return NULL;
	
	size += sizeof(allocated_node); // we need to store our info there, too
	if (size % 4 != 0)
		size += 4 - (size % 4);
		
	assert(size % 4 == 0); // the pointer must be 4-byte aligned
	
	size = MAX(size, sizeof(free_node));
	
	
	free_node* node = find_fit(size);
	
	if (!node)
	{
		printf("No available space found for size %d\n", size);
		Mem_Dump();
		return NULL;
	}
	
	void* p = allocate_free_space(node, size);
	
	printf("Alloc of size %d at address %ld\n", size, (long unsigned int)p % (2 << 15));
	
	
	assert(p);
	return p;
}

int Mem_Free(void *ptr)
{
	if (!ptr)
	{
		return -1;
	}
	
	allocated_node* a = ptr - sizeof(allocated_node);
	if (a->magic_number != CAPEL_MAGIC_NUMBER)
	{
			printf("Bad pointer %p passed to Mem_Free.\n", ptr);
			return -1;	
	}

	Mem_Dump();

	free_node* node = free_allocated_space(a);
	merge_adjacent_space(node);
	
	printf("Free at address %ld\n", (long unsigned int)ptr % (2 << 15));
	
	Mem_Dump();
	
	return 0;
}
void Mem_Dump()
{
	printf("=======FREE MEMORY DUMP=====\n");
	printf("Total size : %d\n", _size);
	printf("Address    : Free\n");
	int i =0; 
	free_node* current = _head;
	for(; current; current = current->next)
	{
		i += current->free;
		printf("%-10lx : %-8d \n", (long unsigned int)current % (2 << 17), current->free);
	}
	printf("Allocated: %d\n", _size - i);
	printf("============================\n");
}



free_node* make_free_node(void* address, int size)
{
	//printf("addr: %p, size: %d", address, size); 
	assert(address < _start + _size);
	free_node* node = address;
	node->next = NULL;
	node->free = size;
	
	return node;
}

free_node* free_allocated_space(allocated_node* node)
{
	free_node* new_node = make_free_node(node, node->size);
	list_insert(new_node);
	return new_node;
}

void* allocate_free_space(free_node * node, int required)
{	
	//printf("\nNode->free: %d, Required: %d, diff: %d\n", node->free, required, node->free - required);
	assert(node->free >= required);

	void* node_addr = (void*) node;
	node_addr += required;
	
	//printf("Pointers: %lx - %lx = %ld ?= %d\n", i_node_addr, i_node, i_node_addr - i_node, required); 

	if (node->free - required > 0) // if we have space left over
	{
		free_node* new_node = make_free_node(node_addr, node->free - required);
		list_insert(new_node);
	}
	
	list_remove(node);
	
	// node is now out of the list and we can replace it
	allocated_node* a = (void*)node;
	a->magic_number = CAPEL_MAGIC_NUMBER;
	a->size = required;
	
	return (void*)a + sizeof(allocated_node); // point them to the space, not the node
}

void list_remove(free_node* node)
{
	assert(node);
	assert(_head);
	//printf("remove node: %p\n", node); 
	
	if (node == _head)
	{
		_head = node->next;
		return;
	}
	
	free_node* current = _head;
	
	while(current->next != node)
	{
		current = current->next;	
	}
	
	current->next = node->next;
}

void list_insert(free_node* node)
{
	assert(node);
	//printf("insert node: %p\n", node); 
	
	node->next = NULL;
	
	if (_policy == P_FIRSTFIT) // we don't sort the list: we just add to the head
	{
		node->next = _head;
		_head = node;
		return;
	}
	
	if (_policy == P_BESTFIT) // we sort the list ascendingly	
	{
		if (_head == NULL)
		{
			_head = node;	
			return;
		}
		if (_head->free > node->free)
		{
			node->next = _head;
			_head = node;
			return;
		}
		
		free_node* current = _head;
		for(; current->next && current->next->free < node->free; current = current->next);
		node->next = current->next;
		current->next = node;
		return;
	}
	
	if (_policy == P_WORSTFIT) // we sort the list descendingly	
	{
		if (_head == NULL)
		{
			_head = node;	
			return;
		}
		if (_head->free < node->free)
		{
			node->next = _head;
			_head = node;
			return;
		}
		
		free_node* current = _head;
		for(; current->next && current->next->free > node->free; current = current->next);
		node->next = current->next;
		current->next = node;
		return;
	}
}

free_node* find_fit(int required)
{
	assert(required >= 16);
	
	free_node* current = _head;
	if (_policy == P_WORSTFIT)
	{
		if (_head->free < required)
			return 0; // the list is sorted with biggest at top: FAIL
		else
			return _head;	
	}
	else if (_policy == P_FIRSTFIT || _policy == P_BESTFIT)
	{
		for(; current != NULL; current = current->next)
		{
			if (current->free >= required)
			{
				return current;	
			}
		}
		// if we reach the end of the list with first or best, we FAIL.
		return 0;
	}
	
	return 0;
}

// this function oddly has two repeated sections of code
// this is because if we are looking to merge nodes, a node can be merged twice:
// once before, and once behind
// so the merge algorithm needs to be run twice
//
// a preassumption of this function is that the free_node list is fully merged already 
// (sans the new node)
void merge_adjacent_space(free_node* node)
{
	assert(node);
	assert(_head);
	
	free_node* current = _head;
	void* temp;
	for(; current; current = current->next)
	{
		if (current == node)
			continue;
		else if (current < node)
		{
			temp = (void*) current;
			if (temp + current->free == node)
			{
				node = merge_nodes(current, node);
				break;	
			}	
		}
		else
		{
			temp = (void*) node;
			if (temp + node->free == current)
			{
				node = merge_nodes(node, current);
				break;
			}
		}
	}
	
	current = _head;
	for(; current; current = current->next)
	{
		if (current == node)
			continue;
		else if (current < node)
		{
			temp = (void*) current;
			if (temp + current->free == node)
			{
				node = merge_nodes(current, node);
				break;	
			}	
		}
		else
		{
			temp = (void*) node;
			if (temp + node->free == current)
			{
				node = merge_nodes(node, current);
				break;
			}
		}
	}
}

free_node* merge_nodes(free_node* a, free_node* b)
{
	printf("Merge: %ld, %ld\n", (long unsigned int)a % (2 << 15), (long unsigned int)b % (2 << 15));
	// a must be < b
	if (a > b)
	{
		free_node * temp = b;
		b = a;
		a = temp;	
	}
	
	list_remove(b);
	list_remove(a);
	
	a->free += b->free;
	a->next = NULL;
	
	b->free = 4345345;
	b->next = (void*)0xdeadbeef; // just to make sure
	
	list_insert(a);
	return a; // b in INVALID now
}
