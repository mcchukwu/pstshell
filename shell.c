#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_COUNT 64
#define DELIM " \t\r\n\a"

/* Function to handle cd and change directory
 */
void handle_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "cd: expected argument\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("cd");
    }
  }
}

/* A function to read user's input and return it's value
 */
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

/* A function to process an input, dividing it into commands
 * and argument, to return as tokens */
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

/* Execute command passed and it's arguments */
void execute_command(char **args) {
  if (args[0] == NULL) {
    return;
  }

  if (strcmp(args[0], "cd") == 0) {
    handle_cd(args);
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

/* Entry point */
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

    execute_command(args);
    free(input);
    free(args);
  }

  return 0;
}
