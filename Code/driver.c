#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  FILE *source = fopen("test.txt", "r");
  lexer_init();
  // printf("lexer initialized %d\n", lookup_table[0].present);
  parser_init();
  // printf("parser initialized\n");

  getStream(source);
  // printf("stream obtained\n");
  // print_token_stream(source);

  FILE *fptr = fopen("grammar.txt", "r");
  if (fptr == NULL) {
    perror("fopen");
  }
  grammar_fill(fptr);
  populate_first_sets();
  print_first_sets();
  // print_grammar();
  free(source);
  printf(".......end.......\n");
} // end of main
