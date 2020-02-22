#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexerDef.h"
#include "parser.h"


int main(int argc, char *argv[])
{
    FILE *source = fopen("test.txt", "r");
    FILE *token_file = fopen("tokens.txt", "w");
    lexer_init();
    parser_init();
    getStream(source);
    TOKEN t;
    while(1){
        t = getNextToken(source);
        if(t.name == END_OF_FILE){
            break;
        }
        else
        {
            if(t.name == LEX_ERROR)
            {
                lexError(lexeme,source);
            }
            else
            {
                if(t.name != DELIM)
                {
                    fprintf(token_file, "%s | ", terminal_string[t.name] ); 
                    switch(t.name)
                    {
                        case NUM:  
                            fprintf(token_file, "%d | ", t.num);
                            break;
                        case RNUM:  
                            fprintf(token_file, "%f | ", t.rnum);
                            break;
                        default:
                            fprintf(token_file, "%s | ", t.str);
                            break;
                    }
                    fprintf(token_file, "%d \n", t.line_no);
                }
            }        
        }
    }   // end of while


    FILE *fptr = fopen("grammar.txt", "r");
    if(fptr == NULL)
    {
        perror("fopen");
    }
    grammar_fill(fptr);

    // print_grammar();

    // printf("dr:  %d mp : %d\n", DRIVER, MAINPROGRAM);

    // get_nullable_set();
    populate_first_sets();
    
    populate_follow_sets();
    
    // ull *fset = get_rule_first_set(grammar[0].head);
    // print_rule_fset(fset);

    // print_first_sets();
    print_follow_sets();
}   // end of main

