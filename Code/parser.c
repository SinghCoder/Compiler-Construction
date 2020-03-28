/***************************************
				|GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/

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
#include "semantic_analyzer.h"
/**
 * @brief Parser initialization
 * - initializes all first and follow_set to be empty
 * - inserts entries for terminals and non-terminals in corresponding hashtables
 * - initializes each entry of parsing table as no_matching_rule
 * - sets presence of an error to be false
 */

void parser_init() {

  init_hash_table(terminal_table);
  init_hash_table(non_terminal_table);

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

/**
 * @brief inserts a symbol at end of the given rule's list
 *
 * @param ptr_tail - tail pointer to rule's linked list
 * @param sym - symbol to be inserted
 */
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

/**
 * @brief Constructs an array of linked list to represent grammar
 *
 * @param fptr
 */
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

/**
 * @brief Get the symbol object corresponding to the given string
 *
 * @return symbol - terminal/non_terminal
 */
symbol get_symbol(char str[]) {
  symbol sym;
  if ((str[0] >= 'A') && (str[0] <= 'Z')) {
	sym.is_terminal = false;
	sym.nt = search_hash_table(non_terminal_table, str);
  } else {

	sym.is_terminal = true;
	char tmp[strlen(str)];
	strcpy(tmp, str);
	for (int i = 0; i < strlen(tmp); i++) {
	  tmp[i] = toupper(tmp[i]);
	}
	sym.t = search_hash_table(terminal_table, tmp);
  }
  return sym;
}

/**
 * @brief Utility to print the grammar
 *
 */

void print_grammar() {
  for (int i = 0; i < NUM_OF_RULES; i++) {
	print_rule(i);
  }
}

/**
 * @brief Find rightmost_set_bit in a given number
 *
 * @return int - position of the bit
 */
int rightmost_set_bit(ull *num) {
  ull temp = (*num & (*num - 1));
  int pos = ULL_NUM_BITS - 1 - log2(temp ^ *num);
  *num = temp;
  return pos;
}

/**
 * @brief Create a parse table using grammar and first and follow sets.
 *
 */
void create_parse_table() {
  for (int i = 0; i < NUM_OF_RULES; i++) {
	// printf("Filling r")
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

/**
 * @brief Do the actual parsing of the source code
 *
 * @param source - source file pointer
 * @return tree_node* - root of the formed parse tree
 */
tree_node *parse_input_source_code(FILE *source) {

  stack *main_stack = stack_init();
  stack *aux_stack = stack_init();
  tree_node *root = create_tree_node();

  root->sym.nt = MAINPROGRAM;
  root->sym.is_terminal = false;
  push(main_stack, root); // push start symbol on stack
  TOKEN tkn = get_next_token(source);

  while (true) {
	tree_node *node = pop(main_stack);
	if ((node != NULL) && (node->sym).is_terminal == true) {
	  if (node->sym.t == EPSILON) {
		  node->token.name = EPSILON;
		  strcpy(node->token.str, "epsilon");
		continue;
	  }
	  if (node->sym.t != tkn.name) // terminal on top of stack does not match
								   // with lookhead symbol
	  {
		// node->token.name = LEX_ERROR;
		char *type_err = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
		sprintf(type_err,"%d) SYNTAX ERORR", tkn.line_no);
		
		char *err_msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
		sprintf(err_msg, "Expected " ANSI_COLOR_YELLOW "\"%s\"," ANSI_COLOR_RESET " Found " ANSI_COLOR_YELLOW "\"%s\" \n" ANSI_COLOR_RESET,terminal_string[node->sym.t], terminal_string[tkn.name]);
		
		print_error(type_err, err_msg);        
		
		printf("Popping token %s from stack top\n",
			   terminal_string[node->sym.t]);
		// tkn = get_next_token(source);
		node = pop(main_stack);
		if (node == NULL) {
		  print_error("Panic Mode Error Recovery Not possible", "Partial parse tree formed");          
		  return root;
		}
		continue;
	  } else { // la token and stack top match
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
		  // node->token.str = (char *)malloc(sizeof(MAX_LEXEME_LEN));
		  strcpy(node->token.str, tkn.str);
		}
	  }

	  tkn = get_next_token(source);
	  continue;
	}

	if (tkn.name == LEX_ERROR) {

	  char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
	  sprintf(err_type, "%d) LEXICAL ERROR", tkn.line_no);
	  print_error(err_type, tkn.str);

	  tkn = get_next_token(source);
	  push(main_stack, node);
	  continue;
	}
	if (node == NULL) {
	  if (tkn.name != DOLLAR) // rule not read completely but stack became empty
	  {
		char *type_err = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
		sprintf(type_err, "%d) SYNTAX ERORR", tkn.line_no);        
		print_error(type_err, "Extra symbols in the source code");        
	  } else {
		printf(
			"\nInput source code is now syntactically correct...........\n\n");
	  }
	  break;
	}

	int rule_no = parse_table[node->sym.nt][tkn.name];
	if (rule_no == NO_MATCHING_RULE) {
	  // printf("[%s][%s]", non_terminal_string[node->sym.nt],
			//  terminal_string[tkn.name]);
	  
	  char *type_err = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
	  sprintf(type_err, "%d) SYNTAX ERORR", tkn.line_no);        
	  print_error(type_err, "No matching rule found in grammar");  

	  printf("Waiting for an element in follow of " ANSI_COLOR_YELLOW "\"%s\"\n" ANSI_COLOR_RESET, non_terminal_string[node->sym.nt]);
	  
	  while (set_find_elem(follow_set[node->sym.nt], tkn.name) == false) {
		tkn = get_next_token(source);
		if (tkn.name == DOLLAR) {
		  print_error("Panic Mode Error Recovery Not possible", "Partial parse tree formed");          
		  return root;
		}
	  }
	  printf("Token \"%s\" found at line number %d\n",
			 terminal_string[tkn.name], tkn.line_no);
	  printf(ANSI_COLOR_GREEN "Resuming parsing\n" ANSI_COLOR_RESET);
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

/**
 * @brief Clear all entries in the grammar linked lists array.
 *
 */
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

/**
 * @brief Print a string as center aligned
 *
 */
void pretty_print(char *s) {
  int column_size = COLUMN_SIZE, len, left_margin;
  len = strlen(s);
  left_margin = (column_size - len) / 2;
  for (int i = 0; i < left_margin; i++) {
	fprintf(parse_tree_file_ptr, " ");
  }
  fprintf(parse_tree_file_ptr, "%s", s);
  int right_margin = left_margin;
  if (len % 2 == 1)
	right_margin++;
  for (int i = 0; i < right_margin; i++) {
	fprintf(parse_tree_file_ptr, " ");
  }
  fprintf(parse_tree_file_ptr, "|");
}

/**
 * @brief Print a node object
 *
 * @param node
 */
void print_node(tree_node *node) {
  char *s = (char *)calloc(30, sizeof(char));
  for (int i = 0; i < 30; i++) {
	s[i] = '\0';
  }

  if (node == NULL)
	return;
  bool is_terminal = (node->sym).is_terminal;
  if (is_terminal == true) {
	if ((node->token.name != NUM && node->token.name != RNUM) &&
		node->token.str != NULL) {
	  sprintf(s, "%s", (node->token).str);
	  pretty_print(s);
	} else
	  pretty_print("----");
	sprintf(s, "%d", (node->token).line_no);
	pretty_print(s);

	if (node->token.str != NULL) {
	  sprintf(s, "%s", terminal_string[(node->token).name]);
	  pretty_print(s);
	} else
	  pretty_print("----");

	switch ((node->token).name) {
	case NUM:
	  sprintf(s, "%d", (node->token).num);
	  pretty_print(s);
	  break;
	case RNUM:
	  sprintf(s, "%f", (node->token).rnum);
	  pretty_print(s);
	  break;
	default:
	  pretty_print("----");
	  break;
	}
	sprintf(s, "%s", non_terminal_string[(node->parent->sym).nt]);
	pretty_print(s);
	pretty_print("yes");
	fprintf(parse_tree_file_ptr, "\t\t%s\n", terminal_string[(node->sym).t]);
  } else {
	pretty_print("----");
	pretty_print("----");
	pretty_print("----");
	pretty_print("----");

	if (node->parent)
	  pretty_print(non_terminal_string[(node->parent->sym).nt]);
	else
	  pretty_print("(ROOT)");
	pretty_print("no");
	fprintf(parse_tree_file_ptr, "\t\t%s\n",
			non_terminal_string[(node->sym).nt]);
  }
}

/**
 * @brief Print the parse tree
 *
 * @param root root node of the tree
 */
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

/**
 * @brief Temp function to test if a given node will be involved in type computation
 * 
 * i.e. if it is involved in an assignment stmt, e.g. EXPRESSION, RELATIONALEXPR, TERM, FACTOR
 */
bool is_expr_node(nonterminal nt){
	switch(nt){
		case EXPRESSION:
		case RELATIONALEXPR:
		case NONUNARYARITHMETICEXPR:
		case UNARYARITHMETICEXPR:
		case TERM:
		case FACTOR:
			return true;
		break;
		default:
			return false;
		break;
	}
}

/**
 * @brief Print a node object
 *
 * @param node
 */
void print_node_for_tool(tree_node *node) {
  if (node == NULL)
	return;
  bool is_terminal = (node->sym).is_terminal;
  if (is_terminal == true) {

	switch ((node->token).name) {
	case NUM:
	  fprintf(parse_tree_file_ptr, "[num %d]", (node->token).num);
	  break;

	case RNUM:
	  fprintf(parse_tree_file_ptr, "[rnum %f]", (node->token).rnum);
	  break;

	default: {
	  char tkn_name[MAX_LEXEME_LEN];
	  strcpy(tkn_name, terminal_string[(node->sym).t]);

	  for (int i = 0; i < strlen(tkn_name); i++)
		tkn_name[i] = tolower(tkn_name[i]);

	  if (node->token.name != SQBO && node->token.name != SQBC)
		fprintf(parse_tree_file_ptr, "[%s %s] ", tkn_name, node->token.str);

	  else
		fprintf(parse_tree_file_ptr, "[%s] ", tkn_name);

	} break;
	}
  } else {    
	fprintf(parse_tree_file_ptr, "[%s(%d)", non_terminal_string[(node->sym).nt], node->num_child);
	if(is_expr_node(node->sym.nt)){ //it's a node corresponding to RHS of an expression
		fprintf(parse_tree_file_ptr, "{%s} ",terminal_string[get_expr_type(node).name]);
	} 
  }
}

/**
 * @brief Print the parse tree
 *
 * @param root root node of the tree
 */
void print_parse_tree_for_tool(tree_node *root) {
  if (root == NULL)
	return;
  print_node_for_tool(root);

  if (root->leftmost_child)
	print_parse_tree_for_tool(root->leftmost_child);
  //   else
  // 	fprintf(parse_tree_file_ptr,"]");

  if (root->leftmost_child) {
	tree_node *temp = root->leftmost_child->sibling;
	while (temp != NULL) {
	  print_parse_tree_for_tool(temp);
	  temp = temp->sibling;
	}
  }
  if (root->sym.is_terminal == false)
	fprintf(parse_tree_file_ptr, "]");
}

/**
 * @brief Print all first sets of the given grammar
 *
 */
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

/**
 * @brief Print the follow sets of the given grammar
 *
 */
void print_follow_sets() {
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) {
	printf("FOLLOW[");
	char *s = non_terminal_string[i];
	printf("%*s%*s", (int)(15 + strlen(s) / 2), s, (int)(15 - strlen(s) / 2),
		   "");
	printf("] = { ");
	for (int j = 0; j < SET_SIZE; j++) {
	  for (int k = 0; k < ULL_NUM_BITS; k++) {
		if (set_find_elem(follow_set[i], j * ULL_NUM_BITS + k) == true) {
		  printf("%s  ", terminal_string[j * ULL_NUM_BITS + k]);
		}
	  }
	}
	printf("}\n");
  }
}

/**
 * @brief Print the first set of a given non-terminal
 *
 */
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

/**
 * @brief Calculate the follow sets of the non-terminals in grammar
 *
 */

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

/**
 * @brief Populate first sets of given non-terminals of the grammar
 *
 */
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
			if (set_find_elem(first_set[lhs], t) ==
				false) // check if terminal already there in the
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

/**
 * @brief Get the first set for a string of terminals and non-terminals
 *
 * @param node starting node of the string
 */
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
/**
 * @brief Print a grammar rule
 *
 * @param rule_no Rule number to be printed
 */
void print_rule(int rule_no) {
  int lhs = grammar[rule_no].lhs;
  rhsnode_ptr head = grammar[rule_no].head;
  printf("%s -> ", non_terminal_string[lhs]);
  while (head != NULL) {
	print_symbol(head->sym);
	printf(" ");
	head = head->next;
  }
  printf("\n");
}

/**
 * @brief Print the parse table entries
 *
 */
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

void print_symbol(symbol sym) {
  if (sym.is_terminal == true) {
	printf("%s", terminal_string[sym.t]);
  } else {
	printf("%s", non_terminal_string[sym.nt]);
  }
}
