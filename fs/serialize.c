#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../chars.h"

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x "\n", __LINE__, __func__, ## args)

static char* serialize_entry(Entry * e) {
  char *n = serialize(e->n);
  printk("%s", n);

  int needed = 1;
  needed += strlen(n);
  needed += strlen("E('',)");
  needed += strlen(e->name);

  char *s = malloc(needed);
  snprintf(s, needed, "E('%s',%s)", e->name, n);
  free(n);

  return s;
}

char* serialize_directory(Directory* dir) {
  size_t needed = strlen("D()") + 1;
  char* strs[ENTRIES_PER_DIR];
  memset(strs, 0, sizeof(strs));

  for(size_t i = 0; i < ENTRIES_PER_DIR; i++) {
    strs[i] = dir->entries[i] ? serialize_entry(dir->entries[i]) : NULL;
    needed += strs[i] ? strlen(strs[i]) + 1 : 0; // 1 for comma
  }

  char* s = malloc(needed);
  char* orig = s;
  snprintf(s, needed, "D(");
  s += strlen("D(");

  for(size_t i = 0; i < ENTRIES_PER_DIR; i++) {
    if (strs[i]) {
      snprintf(s, needed - (s - orig), "%s,", strs[i]);
      s += strlen(strs[i]) + 1;
      free(strs[i]);
    }
  }
  snprintf(s, needed - (s - orig), ")");

  return orig;
}

static char* serialize_block(Block * b) {
  int needed = strlen("B()" + 33);
  char *s = malloc(needed);
  snprintf(s, needed, "B(%zd)", b->bid);
  return s;
}

static char* serialize_link(Link * l) {
  int needed = strlen("L('')" + strlen(l->path) + 1);
  printk("%d", needed);
  char *s = malloc(needed);
  snprintf(s, needed, "L('%s')", l->path);
  return s;
}


static void indent_print(int indent) {
  for(int i = 0; i < indent; i++) {
    printf(" ");
  }
}

static void pretty_print_entry(Entry* e, int indent) {
  if (!e) return;
  indent_print(indent);
  printf("%s: ", e->name);
  pretty_print(e->n, indent);
  printf("\n");
}

void pretty_print(Node* n, int indent) {
  assert(n);
  indent_print(indent);
  switch(n->type) {
    case DIRECTORY:
      printf("\n");
      for (size_t i = 0; i < ENTRIES_PER_DIR; i++) {
        pretty_print_entry(n->dir->entries[i], indent + 2);
      }
      return;
    case LINK:
      printf("L('%s')", n->link->path);
      return;
    case STRING:
      printf("'%s'", n->s);
      return;
    case INTEGER:
      printf("%d", n->i);
      return;
    case BLOCK:
      printf("B(%zd)", n->block->bid);
      return;
    case ENTRY:
      pretty_print_entry(n->e, indent);
      return;
    default:
      assert(0);
  }
}

char* serialize(Node* node) {
  char *s;
  switch (node->type) {
    case DIRECTORY:
      return serialize_directory(node->dir);
    case BLOCK:
      return serialize_block(node->block);
    case LINK:
      return serialize_link(node->link);
    case ENTRY:
      return serialize_entry(node->e);
    case INTEGER:
      s = malloc(35);
      snprintf(s, 35, "%d", node->i);
      return s;
    case STRING:
      s = malloc(strlen(node->s) + 3);
      snprintf(s, strlen(node->s) + 3, "'%s'", node->s);
      return s;
    default:
      assert(0);
  }
}
