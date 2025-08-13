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
    int redirect_out = -1;
    int redirect_in = -1;
    int is_append = 0;

    for (int i = 0; args[i] != NULL; i++) {
      if (strcmp(args[i], ">") == 0) {
        redirect_out = i;
        is_append = 0;
      } else if (strcmp(args[i], ">>") == 0) {
        redirect_out = i;
        is_append = 1;
      } else if (strcmp(args[i], "<") == 0) {
        redirect_in = i;
      }
    }

    pid_t pid = fork();
    if (pid == 0) {
      if (redirect_out != -1) {
        if (args[redirect_out + 1] == NULL) {
          fprintf(stderr, "Syntax error: no file after '%s'\n",
                  args[redirect_out]);
          exit(1);
        }

        int flags = O_WRONLY | O_CREAT;
        if (is_append) {
          flags |= O_APPEND;
        } else {
          flags |= O_TRUNC;
        }

        int fd_out = open(args[redirect_out + 1], flags, 0644);
        if (fd_out < 0) {
          perror("open output file");
          exit(1);
        }

        if (dup2(fd_out, STDOUT_FILENO) < 0) {
          perror("dup2 stdout");
          exit(1);
        }
        close(fd_out);

        args[redirect_out] = NULL;
      }

      if (redirect_in != -1) {
        if (args[redirect_in + 1] == NULL) {
          fprintf(stderr, "Syntax error: no file after '<'\n");
          exit(1);
        }

        int fd_in = open(args[redirect_in + 1], O_RDONLY);
        if (fd_in < 0) {
          perror("open input file");
          exit(1);
        }

        if (dup2(fd_in, STDIN_FILENO) < 0) {
          perror("dup2 stdin");
          exit(1);
        }
        close(fd_in);

        args[redirect_in] = NULL;
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
