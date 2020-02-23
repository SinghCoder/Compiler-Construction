#include "parser.h"
#include "lexer.h"
#include "stackADT.h"
#include "treeADT.h"
#include "hashtable.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parser_init() 
{

  FILE *file = fopen("non_terminals.txt", "r");
  fseek(file, 0, SEEK_END);
  
  int length = ftell(file);
  
  fseek(file, 0, SEEK_SET);
  
  char *temp = malloc(sizeof(char) * (length));

  fread(temp, sizeof(char), length, file);
  fclose(file);
  //initialize all first sets to be null
  for(int i=0; i<NUM_OF_NONTERMINALS; i++)
  {
      // first_set[i] = 0;
      for(int j = 0; j < BITSTRING_PART_NUM ; j++)
      {
          first_set[i][j] = 0;
      }
  }



  char *nt_read;
  int i;
  nt_read = strtok(temp, ", \n");
  
  for (i = 0 ; nt_read != NULL; i++)
  {
    // printf("nt read : %s\n", nt_read);
    strcpy(non_terminal_string[i], nt_read);
    nt_read = strtok(NULL, ", \n");
  }

  fclose(file);

  terminal_table = init_hash_table();
  non_terminal_table = init_hash_table();

  for (i = 0; i < NUM_OF_TERMINALS; i++) 
  {
    // printf("inserting in hash table %s=============================\n", terminal_string[i]);
    // fflush
    hash_insert(terminal_table, terminal_string[i], i);
  }

  for (i = 0; i < NUM_OF_NONTERMINALS; i++) 
  {
    hash_insert(non_terminal_table, non_terminal_string[i], i);
  }
  
  //initializing parse table
  for (int i = 0; i < NUM_OF_NONTERMINALS; i++) 
  {
    for (int j = 0; j < NUM_OF_TERMINALS; j++) 
    {
      parse_table[i][j] = -1;   //initialize all rules to -1
    }
  }
}

void insert_at_end(rhsnode_ptr *ptr_tail, symbol sym) 
{

  rhsnode_ptr node = (rhsnode_ptr)malloc(sizeof(rhsnode));
  node->s = sym;
  node->next = NULL;

  if (*ptr_tail != NULL) 
  {
    (*ptr_tail)->next = node;
  }
  *ptr_tail = node;
}

void grammar_fill(FILE *fptr)
{

  int rule_num = 0;
  char buffer[RHS_MAX_LENGTH];

  while (fgets(buffer, sizeof(buffer), fptr) != NULL) 
  {
    // printf("%s\n",buffer);
    char *sym_read;
    int i;
    sym_read = strtok(buffer, " \n");
    for (i = 0; sym_read != NULL; i++) 
    {
      // printf("%s\n", sym_read);
      if (i == 0) // LHS of a production
      {
        grammar[rule_num].lhs = get_symbol(sym_read).nt;
        // printf("hola................... %d\n", grammar[rule_num].lhs);
        grammar[rule_num].head = NULL;
        grammar[rule_num].tail = NULL;
        // printf("%d\n", grammar[rule_num].sym);
        // printf("LHS: = %d\n", get_symbol( sym_read ).nt);
      } 
      else 
      {
        symbol sym = get_symbol(sym_read);
        insert_at_end(&(grammar[rule_num].tail), sym);
        if (grammar[rule_num].head == NULL) 
        {
          grammar[rule_num].head = grammar[rule_num].tail;
        }
      }
      sym_read = strtok(NULL, " \n");
    }
    // printf("populating rule : %d\n", rule_num);
    // fflush(stdout);
    rule_num++;
  }
}

symbol get_symbol(char str[]) 
{
  // printf("str: %s\n", str);
  symbol s;
  if ((str[0] >= 'A') && (str[0] <= 'Z'))
  {
    s.is_terminal = false;
    s.nt = searchHashTable(non_terminal_table, str);
  } 
  else 
  {
    
    // fflush(stdout);
    s.is_terminal = true;
    char tmp[strlen(str)];
    strcpy(tmp, str);
    for (int i = 0; i < strlen(tmp); i++) 
    {
      tmp[i] = toupper(tmp[i]);
    }
    // printf("scanning terminal %s\n", tmp);
    s.t = searchHashTable(terminal_table, tmp);
    // printf("searching in hashtable %d\n", s.t);
    fflush(stdout);
  }
  return s;
}

void print_grammar() 
{
  for (int i = 0; i < NUM_OF_RULES; i++) 
  {
    printf("%s -> ", non_terminal_string[grammar[i].lhs]);
    fflush(stdout);
    rhsnode_ptr temp = grammar[i].head;
    
    while (temp != NULL) 
    {
      if ( (temp->s).is_terminal == false) 
      {
        printf("%s ", non_terminal_string[(temp->s).nt]);
        fflush(stdout);
      }
      else 
      {
        printf("%s ", terminal_string[(temp->s).t]);
        fflush(stdout);
      }
      temp = temp->next;
    }
    printf("\n");
    fflush(stdout);
  }
}

int rightmost_set_bit(unsigned long long *num) 
{
  unsigned long long temp = (*num & (*num - 1));
  int pos = NUM_BITS - 1 - log2(temp ^ *num);
  *num = temp;
  return pos;
}

void createParseTable() 
{  
  for (int i = 0; i < NUM_OF_RULES; i++) 
  {
    nonterminal nt = grammar[i].lhs;
    // printf("i is %d , nt enum is %d\n",i , nt);
    // printf("nt is %s\n", non_terminal_string[nt]);
    ull* first_set_rhs = get_rule_first_set(grammar[i].head);
    for (int j = 0; j < BITSTRING_PART_NUM; j++) 
    {
      unsigned long long num = first_set_rhs[j];    // first of lhs
      while (num) 
      {
        int pos = rightmost_set_bit(&num); //position of terminal in first set
        pos += (NUM_BITS * j);
        // printf("pos is : %d\n", pos);
        
        // if(pos < NUM_OF_TERMINALS)
          // printf("terminal is %s\n", terminal_string[pos]);

           //

        if (EPSILON == pos) 
        {
          for (int j = 0; j < BITSTRING_PART_NUM; j++) 
          {
            unsigned long long num = follow_set[nt][j];   // if epsilon in first set, add follow also
            while (num) 
            {
              int pos = rightmost_set_bit(&num);
              pos += (NUM_BITS * j);
              if (EPSILON != pos) 
              {
                parse_table[nt][pos] = i;
              }
            }
          }//end of for
        } // end of if - epsilon presence
        else
        {
            parse_table[nt][pos] = i; //don't consider epsilon as terminal
        }
      } // end of while - adding elements of first set
    } // end of for - traversing in all subparts of bitstring
  } // end of for - travwersal in all rules
}

tree_node *parseInputSourceCode(FILE *source) 
{
  stack *main_stack = stack_init();
  stack *aux_stack = stack_init();
  // printf("entered into parsing code\n");
  tree_node *root = create_tree_node();

  root->sym.nt = MAINPROGRAM;
  root->sym.is_terminal = false;
  
  push(main_stack, root); //push start symbol on stack
  // printf("oyeee....\n");
  TOKEN tkn = getNextToken(source);
  // printf("token from la : %s\n", terminal_string[tkn.name]);
  while (true) 
  {
    tree_node *node = pop(main_stack);
	if((node != NULL) && (node->sym).is_terminal == true)
	{
		node->token.line_no = tkn.line_no;
		node->token.name = tkn.name;
		if(tkn.name == NUM)
		{
			node->token.num = tkn.num;
		}
		else if(tkn.name == RNUM)
		{
			node->token.rnum = tkn.rnum;
		}
		else
		{
			node->token.str = tkn.str;
		}
	}

    if (tkn.name == LEX_ERROR) 
    {
      printf("LEX_ERROR\n");
      printf("%d) Lexical Error\n", tkn.line_no);
      tkn = getNextToken(source);
      continue;
    }
    if (node == NULL) 
    { //implement error recovery here
      // printf("stack is empty = node NULL\n");
      if (tkn.name != DOLLAR) // rule not read completely but stack became empty
      {
        // printf("a\n");
        printf("%d) Syntax Error\n", tkn.line_no);
      }
      else
      {
         printf("\nInput source code is syntactically correct...........\n\n");
      }
      break;
    }

    if (node->sym.is_terminal == true) 
    {
      // printf("stack top is terminal\n");
      // printf("%s\n", terminal_string[node->sym.t]);
      // printf("---------------la : %s\n", terminal_string[tkn.name]);
      if (node->sym.t != tkn.name)  // terminal on top of stack does not match with lookhead symbol
      {
        printf("%d) Syntax Error : lookahead char don't match with stack top\n", tkn.line_no);
        break;  // don't break, instead continue parsing after synch token
      }
      tkn = getNextToken(source);
      continue;
    }
    else
    {
      //  printf("stack top is non_terminal\n");
    //    printf("%s\n", non_terminal_string[node->sym.nt]);
    }
    

    int rule_no = parse_table[node->sym.nt][tkn.name];
    // printf("Checking pt for PT[%s][%s]\n", non_terminal_string[node->sym.nt], terminal_string[tkn.name]);
    cell rule = grammar[rule_no];
    rhsnode_ptr rhs_ptr = rule.head;
    
    if(rule_no == -1)
    {
      printf("No matching rule\n");
    }
    else
    {
      // printf("Rule used : \n");
      // print_rule(rule_no);
    }

    // printf("\t|----->");

    while (rhs_ptr != NULL) 
    {
      tree_node *temp = create_tree_node();
      temp->parent = node;
      temp->sym = rhs_ptr->s;
      
      if(temp->sym.is_terminal == true)
      {
        // printf("%s  -->", terminal_string[temp->sym.t]);
        if(temp->sym.t != EPSILON)
        {
            add_child(node, temp);
            push(aux_stack, temp);
        }
      }
      else
      {
        // printf("%s -->", non_terminal_string[temp->sym.nt]);
        add_child(node, temp);
        push(aux_stack, temp);
      }
      rhs_ptr = rhs_ptr -> next;
    //   if(rhs_ptr == NULL)
        // printf("_|_\n");
    }
    
    tree_node *temp = pop(aux_stack);
    
    while (temp != NULL) 
    {
      push(main_stack, temp);
      temp = pop(aux_stack);
    }
  }
  return root;
}
void print_node(tree_node *node) 
{
	printf("inside print node\n");
	bool is_terminal = (node->sym).is_terminal;
	char *s;
	if(is_terminal == true)
	{
		s = (node->token).str;
        printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		printf("  %d  ", (node->token).line_no );
		s = terminal_string[(node->token).name];
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		if((node->token).name == NUM)
		{
			printf("  %d  ", (node->token).num );
		}
		else if((node->token).name == RNUM)
		{
			printf("  %f  ", (node->token).rnum );
		}

		
		s = non_terminal_string[(node->parent->sym).nt];
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");

		
		s = "yes";
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");

		s = non_terminal_string[(node->sym).nt];
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
	}
	else
	{
		s = "----";
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");

		
		s = non_terminal_string[(node->parent->sym).nt];
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
				
		s = "no";
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
		
		s = non_terminal_string[(node->sym).nt];
		printf("%*s%*s",5 + strlen(s)/2 , s ,5 - strlen(s)/2 , "");
	}

	printf("\n");
}	
// 
void print_parse_tree(tree_node *root) 
{
	if(root->sym.is_terminal)
	{
		printf("Current root node : %s\n", terminal_string[root->sym.t]);
	}
	else
	{
		printf("Current root node : %s\n", non_terminal_string[root->sym.nt]);
	}
	if(root == NULL)
	{
		return;
	}
	else
	{	
		print_parse_tree( root->leftmost_child );
		print_node(root);
		tree_node *temp = root->leftmost_child->sibling;
		while(temp != NULL)
		{
			print_parse_tree(temp);
			temp = temp->sibling;
		}
	}
}
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


// ull 

void print_first_sets()
{
    for( int i = 0; i < NUM_OF_NONTERMINALS; i++)
    {
        printf("FIRST[");        
        char *s = non_terminal_string[i];
        printf("%*s%*s",15+strlen(s)/2,s,15-strlen(s)/2,"");
        printf("] = { ");
        for(int j = 0; j< BITSTRING_PART_NUM ; j++)
        {
            for(int k = 0; k < NUM_BITS; k++)
            {
                if((first_set[i][j] & ((1ULL << (NUM_BITS-1) ) >> k)) != 0)
                {
                    // printf(" %d ", j);
                    printf("%s  ", terminal_string[j*NUM_BITS + k]);
                }
            }
        }
        printf("}\n");
        // printf("%llu\n", first_set[i][0]);
    }
}

void print_follow_sets()
{
    for( int i = 0; i < NUM_OF_NONTERMINALS; i++)
    {
        printf("FOLLOW[" );
        char *s = non_terminal_string[i];
        printf("%*s%*s",15+strlen(s)/2,s,15-strlen(s)/2,"");
        printf("] = { ");
        for(int j = 0; j< BITSTRING_PART_NUM ; j++)
        {
            for(int k = 0; k < NUM_BITS; k++)
            {
                if((follow_set[i][j] & ((1ULL << (NUM_BITS-1) ) >> k)) != 0)
                {
                    // printf(" %d ", j);
                    printf("%s  ", terminal_string[j*NUM_BITS + k]);
                }
            }
        }
        printf("}\n");
        // printf("%llu\n", first_set[i][0]);
    }
}

void print_first(nonterminal nt)
{
    // printf("\n{");
    printf("FIRST(%s) = { " , non_terminal_string[nt] );
    for(int j = 0; j< BITSTRING_PART_NUM ; j++)
    {
        for(int k = 0; k < NUM_BITS; k++)
        {
            if((first_set[nt][j] & ((1ULL << (NUM_BITS-1) ) >> k)) != 0)
            {
                // printf(" %d ", j);
                printf("%s, ", terminal_string[j*NUM_BITS + k]);
            }
        }
    }
    printf(" }\n");
}

void print_rule_fset(ull *fset)
{   
    for(int j = 0; j< BITSTRING_PART_NUM ; j++)
    {
        for(int k = 0; k < NUM_BITS; k++)
        {
            if((fset[j] & ((1ULL << (NUM_BITS-1) ) >> k)) != 0)
            {
                // printf(" %d ", j);
                printf("%s, ", terminal_string[j*NUM_BITS + k]);
            }
        }
    }
    printf("\n");
}

void populate_follow_sets()
{
    bool is_changed = true;
    int lhs;
    int rhs_sym;
    rhsnode_ptr rhs_ptr;

    follow_set[MAINPROGRAM][DOLLAR / NUM_BITS] |= ((1ULL << (NUM_BITS-1) ) >> (DOLLAR % NUM_BITS) );

    while(is_changed == true)   //traverse until convergence
    {
        is_changed = false;
        for(int i=0; i<NUM_OF_RULES; i++)
        {
            lhs = grammar[i].lhs;
            rhs_ptr = grammar[i].head;
            rhsnode_ptr temp = rhs_ptr;
            while(temp != NULL)     //traverse till end of the rule
            {
                if( (temp->s).is_terminal == false)
                {
                    rhs_sym = ( (temp->s).nt );
                    // printf("Calculating follow for %s\n", non_terminal_string[rhs_sym]);
                    ull *rhs_rule_set = get_rule_first_set(temp->next);
                    // print_rule_fset(rhs_rule_set);
                    ull *tmp_follow = (ull*)malloc(sizeof(ull) * BITSTRING_PART_NUM);
                    
                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        tmp_follow[j] = follow_set[rhs_sym][j];
                    }
                    
                    bool eps_in_rhs = false;

                    if((rhs_rule_set[EPSILON / NUM_BITS] & ( ((1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) )) != 0)//eps present in this rule
                    {
                        eps_in_rhs = true;
                    }

                    rhs_rule_set[EPSILON / NUM_BITS] &= (~ ((1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) );

                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        follow_set[rhs_sym][j] |= rhs_rule_set[j];
                    }

                    if( (eps_in_rhs == true) || (temp -> next == NULL))
                    {
                        // printf("eps present\n");
                        for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                        {
                            follow_set[rhs_sym][j] |= follow_set[lhs][j];
                        }
                    }

                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        if(follow_set[rhs_sym][j] != tmp_follow[j])
                        {
                            // printf("is changed....\n");
                            is_changed = true;
                        }
                    }
                    // }
                    // follow_set[rhs_sym][EPSILON / NUM_BITS] &= (~ ((1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) );
                }
                temp = temp -> next;
            }   // end of rule linked list traversal while loop
        }   // end of for - grammar traversal
    }   // end of while - infinite loop until convergence
}

void populate_first_sets()
{
    bool is_changed = true;
    int lhs;
    rhsnode_ptr rhs_ptr;
    while(is_changed == true)   //traverse until convergence
    {
        is_changed = false;
        for(int i=0; i<NUM_OF_RULES; i++)
        {
            lhs = grammar[i].lhs;
            rhs_ptr = grammar[i].head;
            if( (rhs_ptr->s).is_terminal == true)  //if terminal, add it and move ahead
            {
                token_name t = (rhs_ptr -> s).t;
                if( ( first_set[lhs][t / NUM_BITS] & ( (1ULL << (NUM_BITS-1) ) >> (t % NUM_BITS) ) ) == 0) //check if terminal already there in the first set or not
                {
                    first_set[lhs][t / NUM_BITS] |= ( (1ULL << (NUM_BITS-1) ) >> (t % NUM_BITS) );
                    is_changed = true;
                }
            }
            else    //nonterminal
            {
                rhsnode_ptr temp = rhs_ptr;
                ull* rhs_symbol_fset;
                ull* lhs_symbol_fset = first_set[lhs];
                while(temp != NULL)     //traverse till end of the rule
                {
                    if((temp->s).is_terminal == true) // if  terminal add and move to next rule
                    {
                        token_name t = (temp -> s).t;
                        if( ( first_set[lhs][t / NUM_BITS] & ( (1ULL << (NUM_BITS-1) ) >> (t % NUM_BITS) ) ) == 0) //check if terminal already there in the first set or not
                        {
                            first_set[lhs][t / NUM_BITS] |= ( (1ULL << (NUM_BITS-1) ) >> (t % NUM_BITS) );
                            is_changed = true;
                        }
                        break;
                    }

                    
                    rhs_symbol_fset = first_set[ (temp -> s).nt ];
                    
                    bool is_diff = false;
                    bool eps_in_rhs = false;
                    bool eps_in_lhs = false;

                    if( ( rhs_symbol_fset[ EPSILON / NUM_BITS ] & ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) ) != 0)  //remove epsilon from current nt before checking things
                    {
                        eps_in_rhs = true;
                        rhs_symbol_fset[ EPSILON / NUM_BITS ] &= (( ~( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS)) )) ;
                    }

                    if( ( lhs_symbol_fset[ EPSILON / NUM_BITS ] & ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) ) != 0)
                    {
                        eps_in_lhs = true;
                    }

                    for(int j = 0; j < BITSTRING_PART_NUM; j++)
                    {
                        if( (lhs_symbol_fset[j] & rhs_symbol_fset[j]) != rhs_symbol_fset[j] )
                        {
                            is_diff = true;
                            break;
                        }
                    }

                    if( is_diff == true )   //rhs nt has a terminal which lhs nt does not have in it's fset
                    {
                        is_changed = true;
                        for(int j = 0; j < BITSTRING_PART_NUM; j++)
                        {
                            lhs_symbol_fset[j] |= rhs_symbol_fset[j];
                        }
                        
                        if( eps_in_rhs == false )   //if rhs nt does not have eps, no need to go further
                        {
                            break;
                        }
                        else
                        {
                            rhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) );  // set eps back to rhs nt
                            if(eps_in_lhs == false)
                            {
                                if(temp->next == NULL)  // only add eps to lhs nt if rhs is last nt in the rule
                                {
                                    lhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) );
                                    is_changed = true;
                                }
                            }
                        }
                    }
                    else    // if is_diff == false, break;
                    {
                        if(eps_in_rhs == true)
                        {
                            rhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) );  // set eps back to rhs nt
                            if(eps_in_lhs == false)
                            {
                                if(temp->next == NULL)  // only add eps to lhs nt if rhs is last nt in the rule
                                {
                                    lhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) );
                                    is_changed = true;
                                }
                            }
                        }
                        break;  //
                    }
                    temp = temp -> next;
                }   // end of rule linked list traversal while loop
            }   // end of else (non-terminal branch)
        }   // end of for - grammar traversal
    }   // end of while - infinite loop until convergence
}   // end of function

ull *get_rule_first_set(rhsnode_ptr node)
{
    // if(node && node->flag == NT)
    //     printf("get_rule start at nt : %s\n", non_terminal_string[(node->s).nt]);
    ull *fset = malloc(sizeof(ull) * BITSTRING_PART_NUM);

    for(int i = 0; i < BITSTRING_PART_NUM; i++)
    {
        fset[i] = 0;
    }

    rhsnode_ptr temp = node;

    int sym;
    while(temp != NULL)
    {
        if( (temp->s).is_terminal == true )
        {
            sym = (temp->s).t;
            fset[sym / NUM_BITS] |= ((1ULL << (NUM_BITS-1) ) >> (sym % NUM_BITS));
            return fset;
        }
        
        else
        {
            sym = (temp->s).nt;

            if( ( first_set[sym][ EPSILON / NUM_BITS ] & ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) ) ) == 0)   //eps not in the nt
            {
                for(int j = 0; j < BITSTRING_PART_NUM; j++)
                {
                    fset[j] |= first_set[sym][j];
                }
                return fset;
            }
            else
            {
                for(int j = 0; j < BITSTRING_PART_NUM; j++)
                {
                    fset[j] |= first_set[sym][j];
                }

                if( temp->next != NULL)
                {
                    fset[ EPSILON / NUM_BITS ] &= (~ ( (1ULL << (NUM_BITS-1) ) >> (EPSILON % NUM_BITS) )); 
                }
            }   // end of else - eps present in fset
        }   // end of else - is nt

        temp = temp -> next;
    }   // end of while - ll traversal
    return fset;
}

void print_rule(int rule_no)
{
    int lhs = grammar[rule_no].lhs;
    rhsnode_ptr head = grammar[rule_no].head;
    printf("%s -> ", non_terminal_string[lhs]);
    while (head != NULL)
    {
      if(( head->s).is_terminal == true)
      {
        printf("%s ", terminal_string[(head->s).t]);
      }
      else
      {
        printf("%s ", non_terminal_string[(head->s).nt]);
      }
      head = head -> next;
    }
    printf("\n");
    
}

void print_parse_table()
{
   for(int i=0; i< NUM_OF_NONTERMINALS; i++)
   {
     for(int j=0; j<NUM_OF_TERMINALS; j++)
     {
       if(parse_table[i][j] != -1)
       {
         printf("parse_table[%s][%s] : ", non_terminal_string[i], terminal_string[j]);
         print_rule(parse_table[i][j]);
       }
     }
    //  printf("\n\n");
   }
}