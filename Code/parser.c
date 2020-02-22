#include "parser.h"
#include "lexer.h"
#include "stackADT.h"
#include "treeADT.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parser_init() {

  FILE *file = fopen("non_terminals.txt", "r");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *temp = malloc(sizeof(char) * (length));
  fread(temp, sizeof(char), length, file);
  fclose(file);

  char *nt_read;
  int i;
  for (i = 0, nt_read = strtok(temp, ", \n"); nt_read != NULL;
       nt_read = strtok(NULL, ", \n"), i++) {
    strcpy(non_terminal_string[i], nt_read);
  }
  fclose(file);

  terminal_table = init_hash_table();
  non_terminal_table = init_hash_table();

  for (i = 0; i < NUM_OF_TERMINALS; i++) {
    hash_insert(terminal_table, terminal_string[i], i);
  }

  for (i = 0; i < NUM_OF_NONTERMINALS; i++) {
    hash_insert(non_terminal_table, non_terminal_string[i], i);
  }

  // createParseTable();
}

void insert_at_end(rhsnode_ptr *ptr_tail, symbol sym) {

  rhsnode_ptr node = (rhsnode_ptr)malloc(sizeof(rhsnode));
  node->s = sym;
  node->next = NULL;

  if (*ptr_tail != NULL) {
    (*ptr_tail)->next = node;
  }
  *ptr_tail = node;
}

void grammar_fill(FILE *fptr) {

  int rule_num = 0;
  char buffer[RHS_MAX_LENGTH];

  while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
    // printf("%s\n",buffer);
    char *sym_read;
    int i;
    for (i = 0, sym_read = strtok(buffer, " \n"); sym_read != NULL;
         sym_read = strtok(NULL, " \n"), i++) {
      // printf("%s\n", sym_read);
      if (i == 0) // LHS of a production
      {
        grammar[rule_num].lhs = get_symbol(sym_read).nt;
        grammar[rule_num].head = NULL;
        grammar[rule_num].tail = NULL;
        // printf("%d\n", grammar[rule_num].sym);
        // printf("LHS: = %d\n", get_symbol( sym_read ).nt);
      } else {
        symbol sym = get_symbol(sym_read);
        insert_at_end(&(grammar[rule_num].tail), sym);
        if (grammar[rule_num].head == NULL) {
          grammar[rule_num].head = grammar[rule_num].tail;
        }
      }
    }
    rule_num++;
  }
}

symbol get_symbol(char str[]) {
  // printf("str: %s\n", str);
  symbol s;
  if ((str[0] >= 'A') && (str[0] <= 'Z')) {
    s.is_terminal = false;
    s.nt = searchHashTable(non_terminal_table, str);
  } else {
    s.is_terminal = true;
    char tmp[strlen(str)];
    strcpy(tmp, str);
    for (int i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }
    s.t = searchHashTable(terminal_table, tmp);
  }
  return s;
}

void print_grammar() {
  for (int i = 0; i < NUM_OF_RULES; i++) {
    printf("%s -> ", non_terminal_string[grammar[i].lhs]);
    rhsnode_ptr temp = grammar[i].head;
    while (temp != NULL) {
      if (temp->s.is_terminal == false) {
        printf("%s ", non_terminal_string[(temp->s).nt]);
      } else {
        printf("%s ", terminal_string[(temp->s).t]);
      }
      temp = temp->next;
    }
    printf("\n");
  }
}

int rightmost_set_bit(unsigned long long *num) {
  unsigned long long temp = (*num & (*num - 1));
  int pos = (sizeof(long long) * 8) - log2(temp ^ *num);
  *num = temp;
  return pos;
}

void createParseTable(long **first, long **follow) {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    for (int j = 0; j < NUM_OF_TERMINALS; j++) {
      parse_table[i][j] = -1;
    }
  }
  for (int i = 0; i < NUM_OF_RULES; i++) {
    nonterminal nt = grammar[i].lhs;
    for (int j = 0; j < 2; j++) {
      unsigned long long num = first[nt][j];
      while (num) {
        int pos = rightmost_set_bit(&num);
        if (j == 1) {
          pos += (sizeof(long long) * 8);
        }
        parse_table[nt][pos] = i;
        if (EPSILON == pos) {
          for (int j = 0; j < 2; j++) {
            unsigned long long num = follow[nt][j];
            while (num) {
              int pos = rightmost_set_bit(&num);
              if (j == 1) {
                pos += (sizeof(long long) * 8);
              }
              parse_table[nt][pos] = i;
            }
          }
        }
      }
    }
  }
}

tree_node *parseInputSourceCode(FILE *source) {
  stack *main_stack = stack_init();
  stack *aux_stack = stack_init();
  tree_node *root = create_tree_node();
  root->sym.nt = MAINPROGRAM;
  root->sym.is_terminal = false;
  push(main_stack, root);

  while (true) {
    TOKEN tkn = getNextToken(source);
    tree_node *node = pop(main_stack);
    if (tkn.name == LEX_ERROR) {
      printf("%d) Lexical Error\n", tkn.line_no);
      continue;
    }
    if (node == NULL) {
      if (tkn.name != END_OF_FILE) {
        printf("%d) Syntax Error\n", tkn.line_no);
      }
      break;
    }

    if (node->sym.is_terminal == true) {
      if (node->sym.t != tkn.name) {
        printf("%d) Syntax Error\n", tkn.line_no);
        break;
      }
      continue;
    }
    int rule_no = parse_table[node->sym.nt][tkn.name];
    cell rule = grammar[rule_no];
    rhsnode_ptr rhs_ptr = rule.head;
    while (rhs_ptr != NULL) {
      tree_node *temp = create_tree_node();
      temp->parent = node;
      temp->sym = rhs_ptr->s;
      add_child(node, temp);
      push(aux_stack, temp);
    }
    tree_node *temp = pop(aux_stack);
    while (temp != NULL) {
      push(main_stack, temp);
      temp = pop(aux_stack);
    }
  }
}
// void print_node(tree_node *ptr,tree_node* end) {
//   if (ptr == end)
//     return;
//   while (ptr != end) {
//     if (ptr->sym.is_terminal == TRUE) {
//       printf("%s\n", terminal_string[ptr->sym.t]);
//     } else {
//       printf("%s\n", non_terminal_string[ptr->sym.nt]);
//     }
//     ptr = ptr->sibling;
//   }
//   return;
// }
// void print_parse_tree(tree_node* root){
//   if(root == NULL){
//     printf("empty root\n");
//     return;
//   }
//   tree_node *curr = root;
//   stack* st = init_stack();
//   if (root->rightmost_child != NULL &&
//     root->rightmost_child != root->leftmost_child)
//   push(st, root->rightmost_child);
//   // if (root->sym.is_terminal == TRUE) {
//   //   printf("%s\n", terminal_string[root->sym.t]);
//   // } else {
//   //   printf("%s\n", non_terminal_string[root->sym.nt]);
//   // }
//   push(st,root);
//   tree_node *ptr = root->leftmost_child;
//   while (ptr!= NULL && ptr != root->rightmost_child)
//   {
//     push(st, ptr);
//     ptr = ptr->sibling;
//   }
//   while(top(st)!=NULL){
//     curr = pop(st);
//     if (curr->rightmost_child != NULL &&
//         curr->rightmost_child != curr->leftmost_child)
//       push(st, curr->rightmost_child);
//     // if (curr->sym.is_terminal == TRUE) {
//     //   printf("%s\n", terminal_string[curr->sym.t]);
//     // } else {
//     //   printf("%s\n", non_terminal_string[curr->sym.nt]);
//     // }
//     push(st,curr);
//     ptr = curr->leftmost_child;
//     while (ptr != NULL && ptr != root->rightmost_child) {
//       push(st, ptr);
//       ptr = ptr->sibling;
//     }
//   }
// }
// void print_parse_tree(tree_node *root) {
//   if (root == NULL) {
//     printf("empty node\n");
//     return;
//   }
//   tree_node *curr = NULL;
//   stack *st = init_stack();
//   if (root->rightmost_child != NULL &&
//       root->rightmost_child != root->leftmost_child)
//     push(st, root->rightmost_child);
//   // print_node(root,root->rightmost_child);
//   if (root->leftmost_child != NULL)
//     push(st, root->leftmost_child);
//   while (top(st) != NULL) {
//     curr = pop(st);
//     if (curr->rightmost_child != NULL &&
//         curr->rightmost_child != curr->leftmost_child)
//       push(st, curr->rightmost_child);
//       {
//         tree_node *temp = curr;
//         while(temp->sibling != curr->rightmost_child){
//           if (temp->rightmost_child != NULL &&
//               temp->rightmost_child != temp->leftmost_child)
//             push(st, temp->rightmost_child);
//           if (temp->sym.is_terminal == TRUE) {
//             printf("%s\n", terminal_string[temp->sym.t]);
//           } else {
//             printf("%s\n", non_terminal_string[temp->sym.nt]);
//           }
//           if (curr->leftmost_child != NULL)
//             push(st, curr->leftmost_child);
//           temp = temp->sibling;
//         }
//       }
//     // print_node(curr,curr->rightmost_child);
//     if (curr->leftmost_child != NULL)
//       push(st, curr->leftmost_child);
//   }
// }

void populate_first_sets() {
  bool is_changed = true;
  int lhs;
  rhsnode_ptr rhs_ptr;
  while (is_changed == true) {
    is_changed = false;
    // for (int i = 0; i < NUM_OF_RULES; i++) {
    //   lhs = grammar[i].lhs;
    //   rhs_ptr = grammar[i].head;
    //   if (rhs_ptr->s.is_terminal == true) {
    //     token_name tkn = (rhs_ptr->s).t;
    //     if ((first_set[lhs][tkn / SIZE_OF_ULL] & (1ULL << (SIZE_OF_ULL -(tkn
    //     % SIZE_OF_ULL+1)))) ==
    //         0) // check if terminal already there in the first set or not
    //     {
    //       first_set[lhs][tkn / SIZE_OF_ULL] |= (1ULL << (SIZE_OF_ULL -(tkn %
    //       SIZE_OF_ULL+1))); is_changed = true;
    //     }
    //   } else {

    // ull *rhs_symbol_fset;
    // ull *lhs_symbol_fset = first_set[lhs];
    for (int i = 0; i < NUM_OF_RULES; i++) {
      lhs = grammar[i].lhs;
      rhs_ptr = grammar[i].head;
      while (rhs_ptr != NULL) {
        if (rhs_ptr->s.is_terminal == true) {
          token_name tkn = (rhs_ptr->s).t;
          if ((first_set[lhs][tkn / SIZE_OF_ULL] &
               (1ULL << (SIZE_OF_ULL - (tkn % SIZE_OF_ULL + 1)))) ==
              0) // check if terminal already there in the first set or not
          {
            first_set[lhs][tkn / SIZE_OF_ULL] |=
                (1ULL << (SIZE_OF_ULL - (tkn % SIZE_OF_ULL + 1)));
            is_changed = true;
          }
          break;
        }

        ull *rhs_first_set = first_set[(rhs_ptr->s).nt];

        for (int j = 0; j < BITSTRING_PART_NUM; j++) {
          if (first_set[lhs][j] != rhs_first_set[j]) {
            first_set[lhs][j] |= rhs_first_set[j];
            is_changed = true;
          }
        }

        if ((rhs_first_set[EPSILON / SIZE_OF_ULL] &
             (1ULL << (SIZE_OF_ULL - (EPSILON % SIZE_OF_ULL + 1)))) == 0) {

          break;
        }

        rhs_ptr = rhs_ptr->next;

        // // bool is_diff = false;
        // if()

        // for (int j = 0; j < BITSTRING_PART_NUM; j++) {
        //   if (first_set[lhs][j] == first_set[j]) {
        //     is_diff = true;
        //     break;
        //   }
        // }

        // if (is_diff == true) {
        //   is_changed = true;
        //   bool eps_in_rhs = false;

        //   // if ((rhs_symbol_fset[EPSILON / SIZE_OF_ULL] &
        //   //      (1ULL << (EPSILON % SIZE_OF_ULL))) != 0) {

        //   //   eps_in_rhs = true;

        //   // }

        //   for (int j = 0; j < BITSTRING_PART_NUM; j++) {
        //     lhs_symbol_fset[j] |= rhs_symbol_fset[j];
        //   }

        //   if (eps_in_rhs) {
        //     rhs_symbol_fset[EPSILON / SIZE_OF_ULL] |=
        //         (1ULL << (EPSILON % SIZE_OF_ULL));
        //     printf("Epsilon added back? : %llu\n",
        //            rhs_symbol_fset[EPSILON / SIZE_OF_ULL] &
        //                (((1ULL << (EPSILON % SIZE_OF_ULL)))));
        //     if (temp->next == NULL) {
        //       lhs_symbol_fset[EPSILON / SIZE_OF_ULL] |=
        //           (1ULL << (EPSILON % SIZE_OF_ULL));
        //     }
        //   } else // if a current nt does not contain epsilon , break
        //   {
        //     break;
        //   }
        // } else // if is_diff == false, break;
        // {
        //   break;
        // }

        // temp = temp->next;
      } // end of rule linked list traversal while loop
      //}   // end of else (non-terminal branch)
    } // end of for - grammar traversal
  }   // end of while - infinite loop until convergence
} // end of function

void print_first_sets() {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    printf("FIRST(%s) = { ", non_terminal_string[i]);
    for (int j = 0; j < BITSTRING_PART_NUM; j++) {
      for (int k = 0; k < SIZE_OF_ULL; k++) {
        if ((first_set[i][j] & (1ULL << (SIZE_OF_ULL - k - 1)) != 0)) {
          // printf(" %d ", j);
          printf("%s, ", terminal_string[j * SIZE_OF_ULL + k]);
        }
      }
    }
    printf(" }\n");
    // printf("%llu\n", first_set[i][0]);
  }
}