#include "parser.h"
#include "hashtable.h"
#include "lexer.h"
#include "setADT.h"
#include "stackADT.h"
#include "treeADT.h"
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void populate_non_terminal_string() {

  FILE *file = fopen("non_terminals.txt", "r");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *nt_file = malloc(sizeof(char) * (length + 1));
  if (nt_file == NULL) {
    perror("parser init failed\n");
    exit(1);
  }

  fread(nt_file, sizeof(char), length, file);
  nt_file[length] = '\0';
  fclose(file);

  char *nt_read = NULL;
  int i;
  nt_read = strtok(nt_file, ", \n");

  for (i = 0; nt_read != NULL; i++) {
    strcpy(non_terminal_string[i], nt_read);
    nt_read = strtok(NULL, ", \n");
  }

  free(nt_file);
}

void parser_init() {

  init_hash_table(terminal_table);
  init_hash_table(non_terminal_table);
  populate_non_terminal_string();

  // initialize all first sets to be null
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    set_init(first_set[i]);
  }

  for (int i = 0; i < NUM_OF_TERMINALS; i++) {
    hash_insert(terminal_table, terminal_string[i], i);
  }

  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    hash_insert(non_terminal_table, non_terminal_string[i], i);
  }

  // initializing parse table
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    for (int j = 0; j < NUM_OF_TERMINALS; j++) {
      parse_table[i][j] = NO_MATCHING_RULE;
    }
  }
}

void insert_at_end(rhsnode_ptr *ptr_tail, symbol sym) {

  rhsnode_ptr node = (rhsnode_ptr)malloc(sizeof(rhsnode));

  if (node == NULL) {
    perror("insertion at end failed\n");
    exit(1);
  }
  node->sym = sym;
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
    char *sym_read;
    int i;

    sym_read = strtok(buffer, " \n");
    for (i = 0; sym_read != NULL; i++) {
      if (i == 0) // LHS of a production
      {
        grammar[rule_num].lhs = get_symbol(sym_read).nt;
        grammar[rule_num].head = NULL;
        grammar[rule_num].tail = NULL;
      } else {
        symbol sym = get_symbol(sym_read);
        insert_at_end(&(grammar[rule_num].tail), sym);
        if (grammar[rule_num].head == NULL) {
          grammar[rule_num].head = grammar[rule_num].tail;
        }
      }
      sym_read = strtok(NULL, " \n");
    }
    rule_num++;
  }
}

symbol get_symbol(char str[]) {
  symbol sym;
  if ((str[0] >= 'A') && (str[0] <= 'Z')) {
    sym.is_terminal = false;
    sym.nt = searchHashTable(non_terminal_table, str);
  } else {

    sym.is_terminal = true;
    char tmp[strlen(str)];
    strcpy(tmp, str);
    for (int i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }
    sym.t = searchHashTable(terminal_table, tmp);
  }
  return sym;
}

void print_grammar() {
  for (int i = 0; i < NUM_OF_RULES; i++) {
    printf("%s -> ", non_terminal_string[grammar[i].lhs]);
    rhsnode_ptr temp = grammar[i].head;

    while (temp != NULL) {
      if ((temp->sym).is_terminal == false) {
        printf("%s ", non_terminal_string[(temp->sym).nt]);

      } else {
        printf("%s ", terminal_string[(temp->sym).t]);
      }
      temp = temp->next;
    }
    printf("\n");
  }
}

int rightmost_set_bit(ull *num) {
  ull temp = (*num & (*num - 1));
  int pos = ULL_NUM_BITS - 1 - log2(temp ^ *num);
  *num = temp;
  return pos;
}

void createParseTable() {
  for (int i = 0; i < NUM_OF_RULES; i++) {

    nonterminal nt = grammar[i].lhs;
    set rhs_first_set = get_rule_first_set(grammar[i].head);

    for (int j = 0; j < SET_SIZE; j++) {
      ull num = rhs_first_set[j]; // first of lhs

      while (num) {
        int pos = rightmost_set_bit(&num); // position of terminal in first set
        pos += (ULL_NUM_BITS * j);
        if (EPSILON == pos) {
          for (int j = 0; j < SET_SIZE; j++) {
            ull num =
                follow_set[nt][j]; // if epsilon in first set, add follow also
            while (num) {
              int pos = rightmost_set_bit(&num);
              pos += (ULL_NUM_BITS * j);
              if (EPSILON != pos) {
                parse_table[nt][pos] = i;
              }
            }
          } // end of for
        }   // end of if - epsilon presence
        else {
          parse_table[nt][pos] = i; // don't consider epsilon as terminal
        }
      } // end of while - adding elements of first set
    }   // end of for - traversing in all subparts of bitstring
  }     // end of for - travwersal in all rules
}

tree_node *parseInputSourceCode(FILE *source) {

  stack *main_stack = stack_init();
  stack *aux_stack = stack_init();
  tree_node *root = create_tree_node();

  root->sym.nt = MAINPROGRAM;
  root->sym.is_terminal = false;
  push(main_stack, root); // push start symbol on stack
  TOKEN tkn = getNextToken(source);

  while (true) {
    tree_node *node = pop(main_stack);
    if ((node != NULL) && (node->sym).is_terminal == true) {
      if (node->sym.t == EPSILON) {
        continue;
      }
      if (node->sym.t != tkn.name) // terminal on top of stack does not match
                                   // with lookhead symbol

      {
        printf(
            "%d) Syntax Error : Lookahead token(%s) doesn't match with stack "
            "top(%s)\n",
            tkn.line_no, terminal_string[tkn.name],
            terminal_string[node->sym.t]);
        tkn = getNextToken(source);
        if (tkn.name == DOLLAR) {
          printf("Panic mode error recovery not possible! Only a partial parse "
                 "tree formed!!\n");
          return root;
        }
        push(main_stack, node);
        continue;
      } else {
        node->token.line_no = tkn.line_no;
        node->token.name = tkn.name;
        switch (tkn.name) {
        case NUM:
          node->token.num = tkn.num;
          break;
        case RNUM:
          node->token.rnum = tkn.rnum;
          break;
        default:
          node->token.str = tkn.str;
        }
      }

      tkn = getNextToken(source);
      continue;
    }

    if (tkn.name == LEX_ERROR) {
      printf("%d) Lexical Error\n", tkn.line_no);
      tkn = getNextToken(source);
      continue;
    }
    if (node == NULL) {
      if (tkn.name != DOLLAR) // rule not read completely but stack became empty
      {
        printf("%d) Syntax Error: Extra symbols in the source code\n",
               tkn.line_no);
      } else {
        printf("\nInput source code is syntactically correct...........\n\n");
      }
      break;
    }

    int rule_no = parse_table[node->sym.nt][tkn.name];

    if (rule_no == NO_MATCHING_RULE) {
      while (set_find_elem(follow_set[node->sym.nt], tkn.name) == false) {
        tkn = getNextToken(source);
        if (tkn.name == DOLLAR) {
          printf("Panic mode error recovery not possible! Only a partial parse "
                 "tree formed!!\n");
          return root;
        }
      }
      continue;
    }
    cell rule = grammar[rule_no];
    rhsnode_ptr rhs_ptr = rule.head;

    while (rhs_ptr != NULL) {
      tree_node *temp = create_tree_node();
      temp->parent = node;
      temp->sym = rhs_ptr->sym;

      add_child(node, temp);
      push(aux_stack, temp);
      rhs_ptr = rhs_ptr->next;
    }

    tree_node *temp = pop(aux_stack);

    while (temp != NULL) {
      push(main_stack, temp);
      temp = pop(aux_stack);
    }
  } // end of while(true) loop : parsing done
  return root;
}

void free_grammar() {
  for (int i = 0; i < NUM_OF_RULES; i++) {
    rhsnode_ptr temp = grammar[i].head, prev;
    while (temp != NULL) {
      prev = temp->next;
      free(temp);
      temp = prev;
    }
  }
}

void print_node(tree_node *node) {
  if (node == NULL)
    return;
  bool is_terminal = (node->sym).is_terminal;
  if (is_terminal == true) {
    printf("%d | %s |", (node->token).line_no,
           terminal_string[(node->token).name]);

    switch ((node->token).name) {
    case NUM:
      printf("  %d | ", (node->token).num);
      break;
    case RNUM:
      printf("  %f | ", (node->token).rnum);
      break;
    default:
      printf(" %s | ", (node->token).str);
      break;
    }

    printf("%s | yes | %s\n", non_terminal_string[(node->parent->sym).nt],
           terminal_string[(node->sym).t]);
  } else {
    printf(" ---- | ---- | ---- | ");

    if (node->parent)
      printf("%s | ", non_terminal_string[(node->parent->sym).nt]);
    else
      printf("%s | ", "(ROOT)");
    printf(" no | %s\n", non_terminal_string[(node->sym).nt]);
  }
}

void print_parse_tree(tree_node *root) {
  if (root == NULL)
    return;

  if (root->leftmost_child)
    print_parse_tree(root->leftmost_child);

  print_node(root);

  if (root->leftmost_child) {
    tree_node *temp = root->leftmost_child->sibling;
    while (temp != NULL) {
      print_parse_tree(temp);
      temp = temp->sibling;
    }
  }
}

void print_first_sets() {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    printf("FIRST[");
    char *s = non_terminal_string[i];
    printf("%*s%*s", (int)(15 + strlen(s) / 2), s, (int)(15 - strlen(s) / 2),
           "");
    printf("] = { ");
    for (int j = 0; j < SET_SIZE; j++) {
      for (int k = 0; k < ULL_NUM_BITS; k++) {
        if (set_find_elem(first_set[i], j * ULL_NUM_BITS + k) == true) {
          printf("%s  ", terminal_string[j * ULL_NUM_BITS + k]);
        }
      }
    }
    printf("}\n");
  }
}

void print_follow_sets() {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    printf("FOLLOW[");
    char *s = non_terminal_string[i];
    printf("%*s%*s", (int)(15 + strlen(s) / 2), s, (int)(15 - strlen(s) / 2),
           "");
    printf("] = { ");
    for (int j = 0; j < SET_SIZE; j++) {
      for (int k = 0; k < ULL_NUM_BITS; k++) {
        if (set_find_elem(follow_set[i],  j * ULL_NUM_BITS + k) == true) {
          printf("%s  ", terminal_string[j * ULL_NUM_BITS + k]);
        }
      }
    }
    printf("}\n");
  }
}

void print_first(nonterminal nt) {
  // printf("\n{");
  printf("FIRST(%s) = { ", non_terminal_string[nt]);
  for (int j = 0; j < SET_SIZE; j++) {
    for (int k = 0; k < ULL_NUM_BITS; k++) {
      if ((first_set[nt][j] & ((1ULL << (ULL_NUM_BITS - 1)) >> k)) != 0) {
        // printf(" %d ", j);
        printf("%s, ", terminal_string[j * ULL_NUM_BITS + k]);
      }
    }
  }
  printf(" }\n");
}

void populate_follow_sets() {
  bool is_changed = true;
  int lhs;
  int rhs_sym;
  rhsnode_ptr rhs_ptr;

  set_add_elem(follow_set[MAINPROGRAM], DOLLAR);

  while (is_changed == true) // traverse until convergence
  {
    is_changed = false;
    for (int i = 0; i < NUM_OF_RULES; i++) {
      lhs = grammar[i].lhs;
      rhs_ptr = grammar[i].head;
      rhsnode_ptr temp = rhs_ptr;
      while (temp != NULL) // traverse till end of the rule
      {
        if ((temp->sym).is_terminal == false) {
          rhs_sym = ((temp->sym).nt);
          set rhs_rule_set = get_rule_first_set(temp->next);
          set tmp_follow = (ull *)malloc(sizeof(ull) * SET_SIZE);

          if (tmp_follow == NULL) {
            perror("Follow set memory allocation failed\n");
            exit(1);
          }

          for (int j = 0; j < SET_SIZE; j++) {
            tmp_follow[j] = follow_set[rhs_sym][j];
          }

          bool eps_in_rhs = false;

          if (set_find_elem(rhs_rule_set, EPSILON) ==
              true) // eps present in this rule
          {
            eps_in_rhs = true;
          }

          set_remove_elem(rhs_rule_set, EPSILON);

          set_union(follow_set[rhs_sym], follow_set[rhs_sym], rhs_rule_set);

          if ((eps_in_rhs == true) || (temp->next == NULL)) {
            set_union(follow_set[rhs_sym], follow_set[rhs_sym],
                      follow_set[lhs]);
          }

          for (int j = 0; j < SET_SIZE; j++) {
            if (follow_set[rhs_sym][j] != tmp_follow[j]) {
              // printf("is changed....\n");
              is_changed = true;
            }
          }
          free(tmp_follow);
        }
        temp = temp->next;
      } // end of rule linked list traversal while loop
    }   // end of for - grammar traversal
  }     // end of while - infinite loop until convergence
}

void populate_first_sets() {
  bool is_changed = true;
  int lhs;
  rhsnode_ptr rhs_ptr;
  while (is_changed == true) // traverse until convergence
  {
    is_changed = false;
    for (int i = 0; i < NUM_OF_RULES; i++) {
      lhs = grammar[i].lhs;
      rhs_ptr = grammar[i].head;
      if ((rhs_ptr->sym).is_terminal ==
          true) // if terminal, add it and move ahead
      {
        token_name t = (rhs_ptr->sym).t;
        if (set_find_elem(first_set[lhs], t) ==
            false) // check if terminal already there in the first set or not
        {
          set_add_elem(first_set[lhs], t);
          is_changed = true;
        }
      } else // nonterminal
      {
        rhsnode_ptr temp = rhs_ptr;
        ull *rhs_symbol_fset;
        ull *lhs_symbol_fset = first_set[lhs];
        while (temp != NULL) // traverse till end of the rule
        {
          if ((temp->sym).is_terminal ==
              true) // if  terminal add and move to next rule
          {
            token_name t = (temp->sym).t;
            if (set_find_elem(first_set[lhs],
                              t)) // check if terminal already there in the
                                  // first set or not
            {
              set_add_elem(first_set[lhs], t);
              is_changed = true;
            }
            break;
          }

          rhs_symbol_fset = first_set[(temp->sym).nt];

          bool is_diff = false;
          bool eps_in_rhs = false;
          bool eps_in_lhs = false;

          if (set_find_elem(rhs_symbol_fset,
                            EPSILON) ==
              true) // remove epsilon from current nt before checking things
          {
            eps_in_rhs = true;
            set_remove_elem(rhs_symbol_fset, EPSILON);
          }

          if (set_find_elem(lhs_symbol_fset, EPSILON) == true) {
            eps_in_lhs = true;
          }

          if (is_superset(lhs_symbol_fset, rhs_symbol_fset) ==
              false) // rhs nt has a terminal which lhs nt does not
                     // have in it's fset
          {
            is_changed = true;
            set_union(lhs_symbol_fset, lhs_symbol_fset, rhs_symbol_fset);

            if (eps_in_rhs ==
                false) // if rhs nt does not have eps, no need to go further
            {
              break;
            } else {
              set_add_elem(rhs_symbol_fset, EPSILON); // set eps back to rhs nt
              if (eps_in_lhs == false) {
                if (temp->next == NULL) // only add eps to lhs nt if rhs is last
                                        // nt in the rule
                {
                  set_add_elem(lhs_symbol_fset, EPSILON);
                  is_changed = true;
                }
              }
            }
          } else // if is_diff == false, break;
          {
            if (eps_in_rhs == true) {
              set_add_elem(rhs_symbol_fset, EPSILON); // set eps back to rhs nt
              if (eps_in_lhs == false) {
                if (temp->next == NULL) // only add eps to lhs nt if rhs is last
                                        // nt in the rule
                {
                  set_add_elem(lhs_symbol_fset, EPSILON);
                  is_changed = true;
                }
              }
            }
            break; //
          }
          temp = temp->next;
        } // end of rule linked list traversal while loop
      }   // end of else (non-terminal branch)
    }     // end of for - grammar traversal
  }       // end of while - infinite loop until convergence
} // end of function

set get_rule_first_set(rhsnode_ptr node) {

  set fset = (set)malloc(sizeof(ull) * SET_SIZE);

  if (fset == NULL) {
    perror("get rule first set failed\n");
    exit(1);
  }

  set_init(fset);
  rhsnode_ptr temp = node;
  int sym;

  while (temp != NULL) {
    if ((temp->sym).is_terminal == true) {
      sym = (temp->sym).t;
      set_add_elem(fset, sym);
      return fset;
    }

    else {
      sym = (temp->sym).nt;
      set_union(fset, fset, first_set[sym]);

      if (set_find_elem(first_set[sym], EPSILON) == false) // eps not in the nt
      {
        return fset;
      } else {
        if (temp->next != NULL) {
          set_remove_elem(fset, EPSILON);
        }
      } // end of else - eps present in fset
    }   // end of else - is nt

    temp = temp->next;
  } // end of while - ll traversal
  return fset;
}

void print_rule(int rule_no) {
  int lhs = grammar[rule_no].lhs;
  rhsnode_ptr head = grammar[rule_no].head;
  printf("%s -> ", non_terminal_string[lhs]);
  while (head != NULL) {
    if ((head->sym).is_terminal == true) {
      printf("%s ", terminal_string[(head->sym).t]);
    } else {
      printf("%s ", non_terminal_string[(head->sym).nt]);
    }
    head = head->next;
  }
  printf("\n");
}

void print_parse_table() {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
    for (int j = 0; j < NUM_OF_TERMINALS; j++) {
      if (parse_table[i][j] != NO_MATCHING_RULE) {
        printf("parse_table[%s][%s] : ", non_terminal_string[i],
               terminal_string[j]);
        print_rule(parse_table[i][j]);
      }
    }
  }
}
