#include "lexer.h"
#include "parser.h"
#include "treeADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  setvbuf(stdout, NULL, _IONBF, 0);
  FILE *source = fopen("test.txt", "r");
  lexer_init();
  // printf("lexer initialized %d\n", lookup_table[0].present);
  parser_init();
  // printf("parser initialized\n");

  getStream(source);
  // printf("stream obtained\n");
  // print_token_stream(source);

  FILE *fptr = fopen("grammar.txt", "r");
  if (fptr == NULL) 
  {
    perror("fopen");
  }
  grammar_fill(fptr);

  populate_first_sets();
    
  populate_follow_sets();
  
  createParseTable();
  // ull *fset = get_rule_first_set(grammar[0].head);
  // print_rule_fset(fset);

  // print_first_sets();
  // print_follow_sets();
  
  // print_grammar();
  // print_parse_table();

   tree_node* ptr = parseInputSourceCode(source);

  // free(source);
  printf(".......end.......\n");
} // end of main
