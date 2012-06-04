#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../chars.h"

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x "\n", __LINE__, __func__, ## args)

/*


static void indent_print(int indent) {
  for(int i = 0; i < indent; i++) {
    printf(" ");
  }
}

void pretty_print(Node* n, int indent) {
  assert(n);
  indent_print(indent);
  switch(n->type) {
    case DIRECTORY:
      printf("\n");
      foreach(Entry*e, e, idx, n->dir->v) {
        pretty_print_entry(e, indent + 2);
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
*/
