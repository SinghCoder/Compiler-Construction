#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexerDef.h"

int main()
{
    FILE *source = fopen("test.txt", "r");
    FILE *token_file = fopen("tokens.txt", "w");
    init();
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
}   // end of main

