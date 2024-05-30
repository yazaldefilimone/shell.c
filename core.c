#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/wait.h>
#include <unistd.h>

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

char *core_read_line(void) {
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS); // We recieved an EOF
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}
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

// ============
// split line
//

#define CORE_TOK_BUFSIZE 64
#define CORE_TOK_DELIM " \t\n\a\r"
char **core_split_line(char *line) {
  int bufsize = CORE_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;
  handler_allocator_error(!tokens);
  token = strtok(line, CORE_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position += 1;
    if (position >= bufsize) {
      bufsize += CORE_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      handler_allocator_error(!tokens);
    }
    token = strtok(NULL, CORE_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

// ============
// bootstrap

int core_bootstrap(char **args) {
  pid_t pid, wpid;
  int status;
  pid = fork();

  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("coreshell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("coreshell");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 0;
}

// ============
// builtin functions
int core_help(char **args);
int core_exit(char **args);
int core_cd(char **args);

char *builtin_commands[] = {"help", "exit", "cd"};

int (*builtin_fun[])(char **) = {&core_help, &core_cd, &core_exit};

int core_num_builtins() { return sizeof(builtin_commands) / sizeof(char *); }
int core_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "core: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("core");
    }
  }
  return 1;
}

int core_help(char **args) {
  int i;
  printf("Yazalde Filimone Core Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < core_num_builtins(); i++) {
    printf("  %s\n", builtin_commands[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int core_exit(char **args) { return 0; }

int core_execute(char **args) {
  int index;
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  for (index = 0; index < core_num_builtins(); index++) {
    if (strcmp(args[0], builtin_commands[index]) == 0) {
      return (*builtin_fun[index])(args);
    }
  }
  return core_bootstrap(args);
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
    line = core_read_line();
    args = core_split_line(line);
    status = core_execute(args);
    free(line);
    free(args);
  } while (status);
}

//==============
// main

int main() {
  core();
  return EXIT_SUCCESS;
}
