#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
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

unsigned long long int get_nullable_set()
{
    unsigned long long int set = 0;
    int mask = 1;
    int sym = 0;
    int flag;

    for(int i=0; i<NUM_OF_RULES; i++)
    {
        flag = (grammar[i].head)->flag;
        if(flag == NT)
        {
            sym = (int)( (grammar[i].head)->s ).nt;
            continue;
        }
        else
        {
            sym = (int)( (grammar[i].head)->s ).t;
            if( sym == EPSILON )
            {
                set |= (mask << grammar[i].sym);
                // printf("%d", sym);
                // printf("%s\n", non_terminal_string[grammar[i].sym]);
            }
        }
    }
    return set;
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
                    printf("%s, ", terminal_string[j*NUM_BITS + k]);
                }
            }
        }
        printf(" }\n");
        // printf("%llu\n", first_set[i][0]);
    }
}

// ull* firstOf(nonterminal nt)
// {
//     return first_set[nt];
// }

// bool is_superset(ull a[BITSTRING_PART_NUM], ull b[BITSTRING_PART_NUM])
// {
//     // for(int i=0; i<BITSTRING_PART_NUM; i++)
//     //     printf("a[%d] = %d, b[%d] = %d\n", i, a[i], i, b[i]);
//     for(int i=0; i<BITSTRING_PART_NUM; i++)
//     {
//         for(int j=0; j<NUM_BITS; j++)
//         {
//             if( ((a[i] & (1ULL << j ) ) == 0) && ( (b[i] & (1ULL << j )) != 0 ) ) //jth bit of a[i] is not set but of b[i] is set => a cannot be superset of b
//             {
//                 // printf("diff is in %s.\n", terminal_string[i*NUM_BITS + j]);
//                 // printf("Differing in : %d, %d\n", a[i], b[i]);
//                 return false;
//             }
//         }
//     }
//     return true;
// }

void populate_first_sets()
{
    // grammar[i].head
    bool is_changed = true;
    int lhs;
    rhsnode_ptr rhs_ptr;
    while(is_changed == true)
    {
        // printf("Iterating over grammar.\n");
        is_changed = false;
        for(int i=0; i<NUM_OF_RULES; i++)
        {
            lhs = grammar[i].sym;
            rhs_ptr = grammar[i].head;

            if(rhs_ptr->flag == T)
            {
                token_name t = (rhs_ptr -> s).t;
                if( ( first_set[lhs][t / NUM_BITS] & ( 1ULL << (t % NUM_BITS) ) ) == 0) //check if terminal already there in the first set or not
                {
                    // printf("Adding term %s to first(%s) - \n", terminal_string[t], non_terminal_string[lhs]);
                    // printf("B4 adding : %d\n", first_set[lhs][t / NUM_BITS] & ( 1ULL << t % NUM_BITS ));
                    first_set[lhs][t / NUM_BITS] |= ( 1ULL << (t % NUM_BITS) );
                    // printf("After adding: %d\n", first_set[lhs][t / NUM_BITS] & ( 1ULL << t % NUM_BITS ));
                    // printf("sizeof(llu) : %lu\n", NUM_BITS);
                    // printf("EPSILON : = %d\n", EPSILON);
                    is_changed = true;
                    // break;
                }
            }
            else
            {
                rhsnode_ptr temp = rhs_ptr;
                ull* rhs_symbol_fset;
                ull* lhs_symbol_fset = first_set[lhs];
                while(temp != NULL)
                {
                    if(temp->flag == T)
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
                    for(int j = 0; j < BITSTRING_PART_NUM; j++)
                    {
                        if( (lhs_symbol_fset[j] & rhs_symbol_fset[j]) != rhs_symbol_fset[j] )
                        {
                            is_diff = true;
                            break;
                        }
                    }

                    if( is_diff == true )
                    {
                        is_changed = true;
                        bool eps_in_rhs = false;
                        bool already_pres = false;

                        if( ( rhs_symbol_fset[ EPSILON / NUM_BITS ] & ( 1ULL << (EPSILON % NUM_BITS) ) ) != 0)
                        {
                            // printf("hi");
                            eps_in_rhs = true;
                            if(( lhs_symbol_fset[ EPSILON / NUM_BITS ] & ( 1ULL << (EPSILON % NUM_BITS) ) ) != 0)
                                already_pres = true;
                            // ull a = 0x00;
                            // printf("%llu\n", (( ( 1ULL << (EPSILON % NUM_BITS)) )));
                            // rhs_symbol_fset[ EPSILON / NUM_BITS ] &= ((ull  )( ~( 1ULL << (EPSILON % NUM_BITS)) )) ;
                        }

                        for(int j = 0; j < BITSTRING_PART_NUM; j++)
                        {
                            lhs_symbol_fset[j] |= rhs_symbol_fset[j];
                        }

                        if( eps_in_rhs)
                        {
                            printf("eps pres");
                            if(eps_in_rhs && (!already_pres))
                                lhs_symbol_fset[ EPSILON / NUM_BITS ] &= (~((ull)pow(2,EPSILON)));

                            if(temp->next == NULL)
                            {
                                lhs_symbol_fset[ EPSILON / NUM_BITS ] |= ( 1ULL << (EPSILON % NUM_BITS) );
                            }
                        }
                    }

                    temp = temp -> next;
                }   // end of rule linked list traversal while loop
            }   // end of else (non-terminal branch)
        }   // end of for - grammar traversal
    }   // end of while - infinite loop until convergence
}   // end of function

