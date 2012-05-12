#define NAME_LEN 8
#define NODES_PER_DIR 4
#define PATH_LEN 32

#include <stdlib.h>

typedef struct Node {
  enum { INVALID, DIRECTORY, BLOCK, LINK } type;
  char name[NAME_LEN];
  union {
    struct Block *block;
    struct Directory *dir;
    struct Link *link;
  };
} Node;

typedef struct Directory { 
  Node* nodes[NODES_PER_DIR];
} Directory;

typedef struct Block {
  size_t bid;
  char* data;
} Block;

typedef struct Link {
  char path[PATH_LEN];
  Node* resolved;
} Link;

Directory* ctor_directory();
Link* ctor_link(char* path);
Block* ctor_block(size_t bid);

void dtor_directory(Directory* d);
void dtor_link(Link* l);
void dtor_block(Block* b);
void dtor_node(Node* node);

Node* dir2Node(char* name, Directory* dir);
Node* block2Node(char* name, Block* block);
Node* link2Node(char* name, Link* link);

char* serialize(Node* n);
Node* parse(char* s, size_t *pos);
void pretty_print(Node * n, int indent);


#define NODE(name, o) _Generic((o), \
    Directory*: dir2Node, \
    Link*: link2Node, \
    Block*: block2Node) \
      (name, o)

#define LINK(dir, name, o) link(dir, NODE(name, o))

int dir_slots_free(Directory* dir);

#define SUCCESS 0
#define E_EXISTS -50
#define E_NOTFOUND -51
#define E_FULL -52

int link(Directory* dir, Node* n);
int unlink(Directory* dir, char* name);
Node* lookup(Directory* dir, char* name);
