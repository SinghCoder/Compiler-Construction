#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char terminal_string[100][50];

void print() {
  for (int i = 0; i < 100; i++) {
    printf("%s a\n", terminal_string[i]);
  }
}

int main() {
  FILE *file = fopen("non_terminals.txt", "r");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *temp = malloc(sizeof(char) * (length));
  fread(temp, sizeof(char), length, file);

  char *tk_read;
  int i;
  for (i = 0, tk_read = strtok(temp, ", \n"); tk_read != NULL;
       tk_read = strtok(NULL, ", \n"), i++) {
    // printf("%s a\n", tk_read);
    strcpy(terminal_string[i], "tk_read");
  }
  // printf("%s a\n", terminal_string[2]);
  print();
  free(temp);
}