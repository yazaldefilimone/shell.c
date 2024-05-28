#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// ===
// hanlder error
//
void handler_allocator_error(int is_invalid) {
  if (is_invalid) {
    fprintf(stderr, "core: error to alloc\n");
    exit(EXIT_FAILURE);
  }
}

// =============
// read line
// We don’t know ahead of time how much text a user will enter into their shell.
// You can’t simply allocate a block and hope they don’t exceed it.
// We need to start with a block, and if they do exceed it, reallocate more
// space.
//

#define CORE_READ_LINE_BLOCK_SIZE 1024
char *read_line(void) {
  int block_size = CORE_READ_LINE_BLOCK_SIZE;
  int position = 0;
  char *block = malloc(sizeof(char) * block_size);

  handler_allocator_error(!block);

  int character;

  while (true) {
    character = getchar();

    if (character == EOF || character == '\n') {
      block[position] = '\0';
      return block;
    }
    block[position] = character;
    position += 1;

    if (position >= block_size) {
      block_size += CORE_READ_LINE_BLOCK_SIZE;
      block = realloc(block, block_size);
      handler_allocator_error(!block);
    }
  }

  // return *'-';
}

//==============
// core
#include <stdio.h>
void core(void) {
  char *line;
  char **args;
  int status;

  do {
    printf(">> ");

  } while (status);
}

//==============
// main

int main() {
  core();
  return EXIT_SUCCESS;
}
