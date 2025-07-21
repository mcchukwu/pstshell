#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_COUNT 64
#define DELIM " \t\r\n\a"

char *read_input() {
  char *input = malloc(MAX_INPUT_SIZE);

  if (!input) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  printf("> ");
  if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
    printf("\n");
    exit(0);
  }

  return input;
}

char **parse_input(char *input) {
  int bufsize = MAX_ARG_COUNT, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(input, DELIM);
  while (token != NULL) {
    tokens[position++] = token;

    token = strtok(NULL, DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void execute_command(char **args) {
  if (args[0] == NULL) {
    return;
  }

  pid_t pid = fork();

  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("Error executing command");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("Fork failed");
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
  }
}

int main() {
  char *input;
  char **args;

  while (1) {
    input = read_input();
    args = parse_input(input);

    // Exit condition
    if (args[0] && strcmp(args[0], "exit") == 0) {
      free(input);
      free(args);

      break;
    }
  }

  execute_command(args);

  return 0;
}
