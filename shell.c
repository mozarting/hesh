#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  while (1) {
    char input[256];
    printf("hesh>> ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("EOF or error occurred\n");
      break;
    }
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      exit(0);
    }
  }
  return 0;
}
