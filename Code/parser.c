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

}

void insert_at_end(rhsnode_ptr *ptr_tail, symbol sym, type_of_sym tag)
{
    if( tag == T)
    {
        printf("T: %s\n", terminal_string[sym.t]);
    }
    else
    {
        printf("NT: %s\n", non_terminal_string[sym.nt]);
    }

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
