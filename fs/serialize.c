
char* serialize_directory(Directory* dir) {
  size_t needed = strlen("D(,,,)") + 1;
  char* n0 = dir->nodes[0] ? serialize(dir->nodes[0]) : "()";
  char* n1 = dir->nodes[1] ? serialize(dir->nodes[1]) : "()";
  char* n2 = dir->nodes[2] ? serialize(dir->nodes[2]) : "()";
  char* n3 = dir->nodes[3] ? serialize(dir->nodes[3]) : "()";

  needed += strlen(n0) + strlen(n1);
  needed += strlen(n2) + strlen(n3);

  char* s = malloc(needed);
  sprintf(s, "D(%s,%s,%s,%s)", n0, n1, n2, n3);
  return s;
}

char* serialize_block(Block * b) {
  char *s = malloc(strlen("B()" + 32));
  sprintf(s, "B(%zd)", b->bid);
  return s;
}

char* serialize_link(Link * l) {
  char *s = malloc(strlen("L()" + strlen(l->path) + 1));
  sprintf(s, "L(%s)", l->path);
  return s;
}


void indent_print(int indent) {
  for(int i = 0; i < indent; i++) {
    printf(" ");
  }
}

void pretty_print(Node* n, int indent) {
  indent_print(indent);
  switch(n->type) {
    case DIRECTORY:
      printf("%s :\n", n->name);
      for (size_t i = 0; i < NODES_PER_DIR; i++) {
        if (n->dir->nodes[i]) {
          pretty_print(n->dir->nodes[i], indent + 2);
        }
      }
      return;
    case LINK:
      printf("%s : %s@\n", n->name, n->link->path);
      return;
    case BLOCK:
      printf("%s : [%zd]\n", n->name, n->block->bid);
      return;
    default:
      assert(0);
  }
}

char* serialize(Node* node) {
  char * tmp;
  switch (node->type) {
    case DIRECTORY:
      tmp = serialize_directory(node->dir);
      break;
    case BLOCK:
      tmp = serialize_block(node->block);
      break;
    case LINK:
      tmp = serialize_link(node->link);
      break;
    default:
      assert(0);
  }
  char * s = malloc(1 + strlen(":") + strlen(node->name) + strlen(tmp));
  sprintf(s, "%s:%s", node->name, tmp);
  free(tmp);

  return s;
}
char* serialize(Node* node) {
  char * tmp;
  switch (node->type) {
    case DIRECTORY:
      tmp = serialize_directory(node->dir);
      break;
    case BLOCK:
      tmp = serialize_block(node->block);
      break;
    case LINK:
      tmp = serialize_link(node->link);
      break;
    default:
      assert(0);
  }
  char * s = malloc(1 + strlen(":") + strlen(node->name) + strlen(tmp));
  sprintf(s, "%s:%s", node->name, tmp);
  free(tmp);

  return s;
}
