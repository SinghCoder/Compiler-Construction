#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

rhsnode_ptr insert(rhsnode_ptr head, symbol sym, type_of_sym tag)
{
    rhsnode_ptr temp = head;
    if(head == NULL)
    {
        temp = (rhsnode_ptr)malloc(sizeof(rhsnode));
        temp->s = sym;
        temp->flag = tag;
        temp->next = NULL;
        return temp;
    }
    
    // printf("insert: %d\n", sym);

    while(temp->next != NULL)
    {
        // perror("hi");
        // printf(" %d\n", temp->s);
        temp = temp->next;
    }

    rhsnode_ptr node = (rhsnode_ptr)malloc(sizeof(rhsnode));
    node->s = sym;
    node->flag = tag;
    node->next = NULL;

    temp->next = node;

    return head;
}

grammar_t get_rules(FILE *fptr)
{
    grammar_t g;
    
    g = (grammar_t)malloc(sizeof(cell) * NUM_OF_RULES);
    
    int rule_index = 0;

    char buffer[MAX_LENGTH];

    while(fgets(buffer,sizeof(buffer), fptr) != NULL)
    {
        // printf("%s\n",buffer);
        char *sym_read;
        int i;
        for( i = 0,sym_read = strtok(buffer, " \n"); sym_read != NULL ; sym_read = strtok(NULL, " \n"), i++) 
        {
            if(i == 0)  //LHS of a production
            {
                g[rule_index].sym = get_symbol_val( sym_read ).nt;
                g[rule_index].head = NULL;
                // printf("%d\n", g[rule_index].sym);
                // printf("LHS: = %d\n", get_symbol_val( sym_read ).nt);
            }
            else
            {
                symbol sym = get_symbol_val(sym_read);
                type_of_sym tag ;
                if(sym_read[0] >= 'A' && sym_read[0] <= 'Z')
                {
                    tag = NT;
                    // printf("RHS: %d\n", get_symbol_val(sym_read).nt);
                }
                else
                {
                    tag = T;
                    // printf("RHS: %d\n", get_symbol_val(sym_read).t);
                }
                g[rule_index].head = insert(g[rule_index].head, sym, tag);
            }
        }

        rule_index++;
        // 
        // break;
    }

    return g;
}
symbol get_symbol_val(char str[])
{
    // printf("str: %s, ", str);
    int isTerminal=0;
    symbol s;
    if(str[0] >='A' && str[0] <= 'Z')
    {
        isTerminal = 0;
    }
    else
    {
        isTerminal = 1;
    }
    if(0 == isTerminal)
    {
        for(int i=0; i < NUM_OF_NONTERMINALS ;i++)
        {
            if(strcmp(str,non_terminal_string[i])==0)
            {
                s.nt = i;
                break;
            }
        }
    }
    else
    {
        for(int i=0; i<strlen(str); i++)
        {
            str[i] = toupper(str[i]);
        }
        for(int i=0; i < NUM_OF_TERMINALS; i++)
        {
            if(strcmp(str,terminal_string[i])==0)
            {
                s.t = i;
                break;
            }
        }
    }
    // printf("fn:%d\n", s);
    return s;
    // here 53 represents the total size of non_terminal_string 
}

void print_grammar(grammar_t g)
{
    for(int i=0; i < NUM_OF_RULES; i++)
    {
        // if()
        printf("%s -> ", non_terminal_string[g[i].sym]);

        rhsnode_ptr temp = g[i].head;
        while(temp != NULL)
        {
            if(temp -> flag == NT)
            {
                printf("%s ", non_terminal_string[(temp -> s).nt]);
            }
            else
            {
                printf("%s ", terminal_string[(temp -> s).t]);
            }
            temp = temp->next;
        }
        printf("\n");
    }
}

int main()
{
    FILE *fptr = fopen("grammar.txt", "r");
    if(fptr == NULL)
    {
        perror("fopen");
    }
    grammar_t g = get_rules(fptr);

    printf("------%s--------\n", terminal_string[EPSILON]);

    print_grammar(g);
}