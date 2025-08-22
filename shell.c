#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void parse_cmd(char *input, char *args[64]) {
    char temp[512];
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '>' || input[i] == '<') {
            if (j > 0 && temp[j - 1] != ' ') {
                temp[j++] = ' ';
            }

            temp[j++] = input[i];

            if (input[i] == '>' && input[i + 1] == '>') {
                temp[j++] = input[++i];
            }

            if (input[i + 1] != ' ' && input[i + 1] != '\0') {
                temp[j++] = ' ';
            }
        } else {
            temp[j++] = input[i];
        }
    }
    temp[j] = '\0';

    char *token = strtok(temp, " \t\n");
    int k = 0;
    while (token != NULL && k < 63) {
        args[k++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }
    args[k] = NULL;
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

void run_pipe(char *input) {
    char *left = strtok(input, "|");
    char *right = strtok(NULL, "|");

    while (*left == ' ') {
        left++;
    }
    while (*right == ' ') {
        right++;
    }

    char *args_left[64];
    int i = 0;
    char *token = strtok(left, " \t\n");
    while (token != NULL && i < 9) {
        args_left[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args_left[i] = NULL;

    char *args_right[10];
    i = 0;
    token = strtok(right, " \t\n");
    while (token != NULL && i < 9) {
        args_right[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args_right[i] = NULL;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(args_left[0], args_left);
        perror("exec left failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        execvp(args_right[0], args_right);
        perror("exec right failed");
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
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

        if (strchr(input, '|')) {
            run_pipe(input);
        } else {
            parse_cmd(input, args);
            if (args[0] == NULL) {
                continue;
            }
            run_cmd(args);
        }
    }

    return 0;
}
