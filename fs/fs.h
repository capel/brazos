#ifndef FS_H
#define FS_H

#define NAME_LEN 8
#define PATH_LEN 32
#define MAX_ARGS 32

#include <stdlib.h>
#include <stdbool.h>
#include <dir.h>

typedef struct Block Block;
typedef const char Link;
typedef struct Directory Directory;
typedef struct Entry Entry;
typedef struct File File;
typedef struct Node Node;
typedef struct fs_state fs_state;


Directory* dir_ctor(Block * b);
File* file_ctor(int size, Block** blocks, int num_args);
Link* link_ctor(const char* path);
Block* block_ctor(int bid);
Entry* entry_ctor(const char* path, Node* n);

#define STYPE_STDIN 0
#define STYPE_STDOUT 1
#define STYPE_BLOCK 3
File* stdin_ctor();
File* stdout_ctor();

// Should be called using DTOR()
void dir_dtor(Directory* d);
void link_dtor(Link* l);
void block_dtor(Block* b);
void entry_dtor(Entry* e);
void file_dtor(File* f);
void node_dtor(Node* n);

char* dir_serialize(Directory* n);
char* link_serialize(Link* n);
char* block_serialize(Block* n);
char* entry_serialize(Entry* n);
char* file_serialize(File* n);
char* node_serialize(Node* n);

#define DTOR(o) do { if (o)  _Generic((o), \
    Directory*: dir_dtor, \
    Link*: link_dtor, \
    Block*: block_dtor, \
    Node*: node_dtor, \
    Entry*: entry_dtor) \
      (o); } while(0)

Node* dir2Node(Directory* d);
Node* link2Node(Link* l);
Node* file2Node(File* f);

bool is_dir(Node* n);
bool is_link(Node* n);
bool is_file(Node* n);

Directory* get_dir(Node *n);
File* get_file(Node *n);
Link* get_link(Node *n);

int node_type(Node* n);

#define NODE(o) _Generic((o), \
    Directory*: dir2Node, \
    Link*: link2Node, \
    File*: file2Node)(o)

Node* walk(const char * path);
Node* root();

int bid_alloc(void);

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


int dir_stat(Directory* dir, int pos, struct _stat_entry* out);

#define Read(b, pos, buf, nbytes) (_Generic((b), \
    File*: file_read, \
    Block*: block_read, \
    Link*: link_read,\
    Node*: node_read))(b, pos, buf, nbytes)

#define Write(b, pos, buf, nbytes) (_Generic((b), \
    File*: file_write, \
    Block*: block_write, \
    Link*: link_write,\
    Node*: node_write))(b, pos, buf, nbytes)

#define Sync(o) (_Generic((o), \
    File*: file_sync, \
    Block*: block_sync, \
    Directory*: dir_sync,\
    Link*: link_sync,\
    Node*: node_sync))(o)

#define Size(o) (_Generic((o), \
    File*: file_size, \
    Block*: block_size, \
    Directory*: dir_size,\
    Link*: link_size,\
    Node*: node_size))(o)

int file_read(File* b, size_t pos, void *buf, size_t nbytes);
int dir_read(Directory* d, size_t pos, void *buf, size_t nbytes);
int link_read(Link* b, size_t pos, void *buf, size_t nbytes);
int block_read(Block* b, size_t pos, void *buf, size_t nbytes);
int node_read(Node* b, size_t pos, void *buf, size_t nbytes);

int block_write(Block* b, size_t pos, const void *buf, size_t nbytes);
int file_write(File* b, size_t pos, const void *buf, size_t nbytes);
int link_write(Link* b, size_t pos, const void *buf, size_t nbytes);
int node_write(Node* b, size_t pos, const void *buf, size_t nbytes);
// always fails
int dir_write(Directory* b, size_t pos, const void *buf, size_t nbytes);

int dir_sync(Directory* d);
int block_sync(Block* b);
int file_sync(File* b);
int link_sync(Link* b);
int node_sync(Node* b);

int dir_size(Directory* d);
int block_size(Block* b);
int file_size(File* b);
int link_size(Link* b);
int node_size(Node* b);

#endif
