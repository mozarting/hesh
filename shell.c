#include <fcntl.h>
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

void run_cmd(char *args[]) {
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
    int redirect = -1;
    for (int i = 0; args[i] != NULL; i++) {
      if (strcmp(args[i], ">") == 0) {
        redirect = i;
        break;
      }
    }

    pid_t pid = fork();
    if (pid == 0) {
      if (redirect != -1) {
        if (args[redirect + 1] == NULL) {
          fprintf(stderr, "Syntax error: no file after '>'\n");
          exit(1);
        }

        int fd = open(args[redirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
          perror("open");
          exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
          perror("dup2");
          exit(1);
        }
        close(fd);

        args[redirect] = NULL;
      }

      execvp(args[0], args);
      perror("exec failed");
      exit(1);
    } else if (pid > 0) {
      wait(NULL);
    } else {
      perror("fork failed");
    }
  }
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
    run_cmd(args);
  }

  return 0;
}
