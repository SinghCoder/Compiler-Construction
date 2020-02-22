#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void parser_init()
{
    char non_terminal_string_copy[NUM_OF_NONTERMINALS][25] = {
        "MAINPROGRAM",
        "MODULEDECLARATIONS",
        "MODULEDECLARATION",
        "OTHERMODULES",
        "DRIVERMODULE",
        "NTMODULE",
        "MODULEDEF",
        "RET",
        "INPUT_PLIST",
        "INPUT_PLIST_DASH",
        "OUTPUT_PLIST",
        "OUTPUT_PLIST_DASH",
        "DATATYPE",
        "TYPE",
        "STATEMENTS",
        "STATEMENT",
        "IOSTMT",
        "LITERAL",
        "VAR",
        "WHICHID",
        "SIMPLESTMT",
        "ASSIGNMENTSTMT",
        "WHICHSTMT",
        "LVALUEIDSTMT",
        "LVALUEARRSTMT",
        "INDEX",
        "MODULEREUSESTMT",
        "OPTIONAL",
        "IDLIST",
        "IDLIST_DASH",
        "EXPRESSION",
        "RESTEXPRESSION",
        "RELATIONALEXPR",
        "RESTRELATIONALEXPR",
        "LOGICALOP",
        "RELATIONALOP",
        "UNARYARITHMETICEXPR",
        "RESTUNARYARITHMETICEXPR",
        "NONUNARYARITHMETICEXPR",
        "ARITHMETICEXPRDASH",
        "TERM",
        "TERM_DASH",
        "FACTOR",
        "ADDSUBOP",
        "MULDIVOP",
        "DECLARESTMT",
        "CONDITIONALSTMT",
        "CASESTMT",
        "OTHERCASE",
        "VALUE",
        "DEFAULTSTMT",
        "ITERATIVESTMT",
        "RANGE"
    };

    for( int i=0 ; i<NUM_OF_NONTERMINALS; i++)
    {
        strcpy(non_terminal_string[i], non_terminal_string_copy[i]);
    }


    //initialize all first sets to be null
    for(int i=0; i<NUM_OF_NONTERMINALS; i++)
    {
        // first_set[i] = 0;
        for(int j = 0; j < BITSTRING_PART_NUM ; j++)
        {
            first_set[i][j] = 0;
        }
    }
}

void insert_at_end(rhsnode_ptr *ptr_tail, symbol sym, type_of_sym tag)
{
//     if( tag == T)
//     {
//         printf("T: %s\n", terminal_string[sym.t]);
//     }
//     else
//     {
//         printf("NT: %s\n", non_terminal_string[sym.nt]);
//     }

    fflush(stdout);

    rhsnode_ptr node = (rhsnode_ptr)malloc(sizeof(rhsnode));
    node->s = sym;
    node->flag = tag;
    node->next = NULL;

    if(*ptr_tail != NULL)
    {
       (*ptr_tail)->next = node;
    }
    *ptr_tail = node;
    // return head;
}

void grammar_fill(FILE *fptr)
{
    
    // grammar = (grammar_t)malloc(sizeof(cell) * NUM_OF_RULES);
    
    int rule_index = 0;

    char buffer[MAX_LENGTH];

    while(fgets(buffer,sizeof(buffer), fptr) != NULL)
    {
        // printf("%s\n",buffer);
        char *sym_read;
        int i;
        for( i = 0,sym_read = strtok(buffer, " \n"); sym_read != NULL ; sym_read = strtok(NULL, " \n"), i++) 
        {
            // printf("%s\n", sym_read);
            if(i == 0)  //LHS of a production
            {
                grammar[rule_index].sym = get_symbol_val( sym_read ).nt;
                grammar[rule_index].head = NULL;
                grammar[rule_index].tail = NULL;
                // printf("%d\n", grammar[rule_index].sym);
                // printf("LHS: = %d\n", get_symbol_val( sym_read ).nt);
            }
            else
            {
                symbol sym = get_symbol_val(sym_read);
                type_of_sym tag ;
                // printf("%c\n", sym_read[0]);
                if((sym_read[0] >= 'A') && (sym_read[0] <= 'Z'))
                {
                    tag = NT;
                    // printf("RHS: %d\n", get_symbol_val(sym_read).nt);
                }
                else
                {
                    // printf("hi\n");
                    tag = T;
                    // if(sym.t == 32763)
                        // printf("----------------%s-------------------", sym_read);
                    // printf("RHS: %d\n", get_symbol_val(sym_read).t);
                }
                insert_at_end(&(grammar[rule_index].tail), sym, tag);
                if(grammar[rule_index].head == NULL)
                {
                    grammar[rule_index].head = grammar[rule_index].tail;
                }
            }
        }

        rule_index++;
        // 
        // break;
    }

    // return g;
}
symbol get_symbol_val(char str[])
{
    // printf("str: %s\n", str);
    bool isTerminal=0;
    symbol s;
    if((str[0] >= 'A') && (str[0] <= 'Z'))
    {
        isTerminal = false;
    }
    else
    {
        isTerminal = true;
    }
    if(false == isTerminal)
    {
        for(int i=0; i < NUM_OF_NONTERMINALS ;i++)
        {
            if(strcmp(str,non_terminal_string[i]) == 0)
            {
                s.nt = i;
                break;
            }
        }
    }
    else
    {
        // if(strcmp(str, "epsilon") == 0)
        // {
        //     printf("------------------------------------------------------------\n");
        // }
        char tmp[strlen(str)];
        strcpy(tmp, str);
        for(int i=0; i<strlen(tmp); i++)
        {
            tmp[i] = toupper(tmp[i]);
        }
        // printf("%s\n", str);
        for(int i=0; i < NUM_OF_TERMINALS; i++)
        {
            if(strcmp(tmp,terminal_string[i]) == 0)
            {
                s.t = i;
                // printf("%d\n", s.t);
                break;
            }
        }
        
    }
    // printf("fn:%d\n", s);
    return s;
    // here 53 represents the total size of non_terminal_string 
}

void print_grammar()
{
    for(int i=0; i < NUM_OF_RULES; i++)
    {
        // if()
        printf("%s -> ", non_terminal_string[grammar[i].sym]);

        rhsnode_ptr temp = grammar[i].head;
        while(temp != NULL)
        {
            if(temp -> flag == NT)
            {
                printf("%s ", non_terminal_string[(temp -> s).nt]);
            }
            else
            {
                // printf("%d\n", (temp->s).t);
                printf("%s ", terminal_string[(temp -> s).t]);
            }
            temp = temp->next;
        }
        printf("\n");
    }
}

// ull 

void print_first_sets()
{
    for( int i = 0; i < NUM_OF_NONTERMINALS; i++)
    {
        printf("FIRST(%s) = { " , non_terminal_string[i] );
        for(int j = 0; j< BITSTRING_PART_NUM ; j++)
        {
            for(int k = 0; k < NUM_BITS; k++)
            {
                if((first_set[i][j] & (1ULL << k)) != 0)
                {
                    // printf(" %d ", j);
                    printf("%s  ", terminal_string[j*NUM_BITS + k]);
                }
            }
        }
        printf(" }\n");
        // printf("%llu\n", first_set[i][0]);
    }
}

void print_follow_sets()
{
    for( int i = 0; i < NUM_OF_NONTERMINALS; i++)
    {
        printf("FOLLOW(%s) = { " , non_terminal_string[i] );
        for(int j = 0; j< BITSTRING_PART_NUM ; j++)
        {
            for(int k = 0; k < NUM_BITS; k++)
            {
                if((follow_set[i][j] & (1ULL << k)) != 0)
                {
                    // printf(" %d ", j);
                    printf("%s  ", terminal_string[j*NUM_BITS + k]);
                }
            }
        }
        printf(" }\n");
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
            if((first_set[nt][j] & (1ULL << k)) != 0)
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
            if((fset[j] & (1ULL << k)) != 0)
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

    follow_set[MAINPROGRAM][DOLLAR / NUM_BITS] |= (1ULL << (DOLLAR % NUM_BITS) );

    while(is_changed == true)   //traverse until convergence
    {
        is_changed = false;
        for(int i=0; i<NUM_OF_RULES; i++)
        {
            lhs = grammar[i].sym;
            rhs_ptr = grammar[i].head;
            rhsnode_ptr temp = rhs_ptr;
            while(temp != NULL)     //traverse till end of the rule
            {
                if(temp->flag == NT)
                {
                    rhs_sym = ( (temp->s).nt );
                    ull *rhs_rule_set = get_rule_first_set(temp->next);
                    ull *tmp_follow = (ull*)malloc(sizeof(ull) * BITSTRING_PART_NUM);

                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        tmp_follow[j] = follow_set[rhs_sym][j];
                    }
                    
                    bool eps_in_rhs = false;

                    if(rhs_rule_set[EPSILON / NUM_BITS] & ( (1ULL << (EPSILON % NUM_BITS) ) ) != 0)//eps present in this rule
                    {
                        eps_in_rhs = true;
                    }

                    rhs_rule_set[EPSILON / NUM_BITS] &= (~ (1ULL << (EPSILON % NUM_BITS) ) );

                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        follow_set[rhs_sym][j] |= rhs_rule_set[j];
                    }

                    if(eps_in_rhs == true)
                    {
                        for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                        {
                            follow_set[rhs_sym][j] |= follow_set[lhs][j];
                        }
                    }

                    for(int j = 0; j < BITSTRING_PART_NUM ; j++)
                    {
                        if(follow_set[rhs_sym][j] != tmp_follow[j])
                            is_changed = true;
                    }
                    // }
                    // follow_set[rhs_sym][EPSILON / NUM_BITS] &= (~ (1ULL << (EPSILON % NUM_BITS) ) );
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
            lhs = grammar[i].sym;
            rhs_ptr = grammar[i].head;
            if(rhs_ptr->flag == T)  //if terminal, add it and move ahead
            {
                token_name t = (rhs_ptr -> s).t;
                if( ( first_set[lhs][t / NUM_BITS] & ( 1ULL << (t % NUM_BITS) ) ) == 0) //check if terminal already there in the first set or not
                {
                    first_set[lhs][t / NUM_BITS] |= ( 1ULL << (t % NUM_BITS) );
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
                    if(temp->flag == T) // if  terminal add and move to next rule
                    {
                        token_name t = (temp -> s).t;
                        if( ( first_set[lhs][t / NUM_BITS] & ( 1ULL << (t % NUM_BITS) ) ) == 0) //check if terminal already there in the first set or not
                        {
                            first_set[lhs][t / NUM_BITS] |= ( 1ULL << (t % NUM_BITS) );
                            is_changed = true;
                        }
                        break;
                    }

                    
                    rhs_symbol_fset = first_set[ (temp -> s).nt ];
                    
                    bool is_diff = false;
                    bool eps_in_rhs = false;
                    bool eps_in_lhs = false;

                    if( ( rhs_symbol_fset[ EPSILON / NUM_BITS ] & ( 1ULL << (EPSILON % NUM_BITS) ) ) != 0)  //remove epsilon from current nt before checking things
                    {
                        eps_in_rhs = true;
                        rhs_symbol_fset[ EPSILON / NUM_BITS ] &= (( ~( 1ULL << (EPSILON % NUM_BITS)) )) ;
                    }

                    if( ( lhs_symbol_fset[ EPSILON / NUM_BITS ] & ( 1ULL << (EPSILON % NUM_BITS) ) ) != 0)
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
                            rhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( 1ULL << (EPSILON % NUM_BITS) );  // set eps back to rhs nt
                            if(eps_in_lhs == false)
                            {
                                if(temp->next == NULL)  // only add eps to lhs nt if rhs is last nt in the rule
                                {
                                    lhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( 1ULL << (EPSILON % NUM_BITS) );
                                    is_changed = true;
                                }
                            }
                        }
                    }
                    else    // if is_diff == false, break;
                    {
                        if(eps_in_rhs == true)
                        {
                            rhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( 1ULL << (EPSILON % NUM_BITS) );  // set eps back to rhs nt
                            if(eps_in_lhs == false)
                            {
                                if(temp->next == NULL)  // only add eps to lhs nt if rhs is last nt in the rule
                                {
                                    lhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( 1ULL << (EPSILON % NUM_BITS) );
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
    ull *fset = malloc(sizeof(ull) * BITSTRING_PART_NUM);

    for(int i = 0; i < BITSTRING_PART_NUM; i++)
    {
        fset[i] = 0;
    }

    rhsnode_ptr temp = node;

    int sym;
    while(temp != NULL)
    {
        if( temp->flag == T)
        {
            sym = (temp->s).t;
            fset[sym / NUM_BITS] |= (1ULL << (sym % NUM_BITS));
            return fset;
        }
        
        else
        {
            sym = (temp->s).nt;

            if( ( first_set[sym][ EPSILON / NUM_BITS ] & ( 1ULL << (EPSILON % NUM_BITS) ) ) == 0)   //eps not in the nt
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
                    fset[ EPSILON / NUM_BITS ] &= (~ ( 1ULL << (EPSILON % NUM_BITS) )); 
                }
            }   // end of else - eps present in fset
        }   // end of else - is nt

        temp = temp -> next;
    }   // end of while - ll traversal
    return fset;
}
