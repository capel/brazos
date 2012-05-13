#define NAME_LEN 8
#define ENTRIES_PER_DIR 8
#define PATH_LEN 32
#define MAX_ARGS ENTRIES_PER_DIR

#include <stdlib.h>

typedef struct Node {
  enum { EMPTY, DIRECTORY, BLOCK, LINK, ENTRY, INTEGER, STRING } type;
  union {
    struct Block *block;
    struct Directory *dir;
    struct Link *link;
    struct Entry *e;
    char* s;
    int i;
  };
} Node;

typedef struct Entry {
  char name[NAME_LEN];
  Node * n;
} Entry;

typedef struct Directory { 
  Entry* entries[ENTRIES_PER_DIR];
} Directory;

typedef struct Block {
  size_t bid;
  char* data;
} Block;

typedef struct Link {
  char* path;
} Link;


Directory* ctor_directory(Node** args);
Link* ctor_link(Node* path);
Block* ctor_block(Node* n);
Entry* ctor_entry(Node* name, Node* n);
Node* ctor_node(void* data, int type);

// Should be called using DTOR()
void dtor_directory(Directory* d);
void dtor_link(Link* l);
void dtor_block(Block* b);
void dtor_entry(Entry* e);
void dtor_node(Node* node);

char* serialize(Node* n);
Node* parse(const char* s);
void pretty_print(Node * n, int indent);
void nop(int i);

#define DTOR(o) do { if (o)  _Generic((o), \
    Directory*: dtor_directory, \
    Link*: dtor_link, \
    Block*: dtor_block, \
    Node*: dtor_node, \
    char*: free, \
    int: nop, \
    Entry*: dtor_entry) \
      (o); } while(0)

#define TAG(o) _Generic((o), \
    Directory*: DIRECTORY,\
    Link*: LINK,\
    Block*: BLOCK,\
    Entry*: ENTRY,\
    char*: STRING,\
    int: INTEGER)

// This creates a Node* from the given data.
// The new node takes exclusive ownership of the data, 
// and will destory it when it is destroyed.
#define NODE(o) ctor_node((void*)(o), TAG((o)))

char* serialize_directory(Directory* dir);
Node* walk(Directory* d, const char * path);

#define SUCCESS 0
#define E_EXISTS -50
#define E_NOTFOUND -51
#define E_FULL -52
#define E_INVAL -53
#define E_CANT -54
#define E_BADFD -55

// Adds a node to the directory with the specified name.
// It takes ownership of the Node.
// Returns:
// E_FULL: No space left in directory
// E_EXISTS: A node with that name already exists
// SUCCESS
int dir_add(Directory* dir, const char* name, Node* n);

// Removes the node with the name from the directory.
// It then destroys the node. If this is not the intended behavior,
// use dir_move to transfer ownership.
// Returns
// E_NOTFOUND: No node by that name was found
// SUCCESS
int dir_remove(Directory* dir, const char* name);


// Transfers ownership of the node with the given name from src to dst.
// E_NOTFOUND: No node by that name was found in src
// Returns
// E_FULL: No space left in dst
// E_EXISTS: A node with that name already exists in dst
// SUCCESS
int dir_move(Directory* src, Directory* dst, const char* name);

// Looks up a name in dir, and returns a node it finds.
// Returns NULL if nothing was found.
// Returns Node* on success
Node* dir_lookup(Directory* dir, const char* name);


int block_read(Block* b, size_t pos, void *buf, size_t nbytes);
int block_write(Block* b, size_t pos, const void *buf, size_t nbytes);
int block_sync(Block* b);

Node* link_resolve(Link* l);
Directory* root(void);
