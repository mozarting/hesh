#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char *prompt = "$ 🍋 >> ";
  char input[256];
  char *args[64];

  while (1) {
    printf("%s", prompt);
    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("EOF or error occurred\n");
      break;
    }

    input[strcspn(input, "\n")] = '\0';
    char *token = strtok(input, " \t\n");

    int i = 0;
    while (token != NULL && i < 64) {
      args[i++] = token;
      token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
      execvp(args[0], args);
      perror("exec failed");
      exit(1);
    } else {
      wait(NULL);
    }
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      exit(0);
    }
  }

  return 0;
}
