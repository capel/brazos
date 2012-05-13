#include "fs.h"

Node* link_resolve(Link* l) {
  return walk(root(), l->path);
}
