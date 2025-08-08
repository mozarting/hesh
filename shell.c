#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void parse_cmd(char *input, char *args[64]) {
  char *token = strtok(input, " \t\n");

  int i = 0;
  while (token != NULL && i < 64) {
    args[i++] = token;
    token = strtok(NULL, " \t\n");
  }

  args[i] = NULL;
}

int main() {
  char pwd[256];
  char *args[64];
  char input[256];

  while (1) {
    getcwd(pwd, sizeof(pwd));

    char prompt[300];
    snprintf(prompt, sizeof(prompt), "%s >> ", pwd);
    printf("%s", prompt);

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("EOF or error occurred\n");
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    parse_cmd(input, args);
    if (args[0] == NULL) {
      continue;
    }

    if (strcmp(args[0], "exit") == 0) {
      printf("exit\n");
      exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
      if (args[1] != NULL) {
        if (chdir(args[1]) != 0) {
          perror("cd failed");
        }
      } else {
        chdir(getenv("HOME"));
      }
    } else {
      pid_t pid = fork();
      if (pid == 0) {
        execvp(args[0], args);
        perror("exec failed");
        exit(1);
      } else {
        wait(NULL);
      }
    }
  }

  return 0;
}
