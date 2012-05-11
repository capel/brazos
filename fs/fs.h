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

// Node wrapper

Node* dir2Node(char* name, Directory* dir);
Node* block2Node(char* name, Block* block);
Node* link2Node(char* name, Link* link);

char* serialize(Node* n);

Node* parse(char* s, size_t *pos);

void dtor_node(Node* node);
void pretty_print(Node * n, int indent);
