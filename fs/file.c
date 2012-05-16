#include "fs.h"
#include <assert.h>
#include <string.h>

struct File {
  size_t num_blocks;
  Block** blocks;
};
