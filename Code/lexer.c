#include <stdio.h>
#include <ctype.h>
#include "lexerDef.h"

void lexError(char *errStr, int line_no)
{
    printf("%d ) Lexical Error : %s\n", line_no, errStr);
}

void init()
{
    state = 0;
    buffer[BUFFER_SIZE]
}

FILE *getStream(FILE *fp)
{
    return fp;
}

TOKEN getNextToken()
{
    char c = getchar1();
    while(c != EOF) {
        switch(state): {
            case 0: 
                if(isAlpha(c)) {
                    state = 1;
                }
                else if(isDigit(c)) {
                        state = 3;
                }
                else if('.' == c) {
                    state = 38;
                }
                else if(';' == c) {
                    state = 40;
                }
                else if(',' == c) {
                    state = 41;
                }
                else if('[' == c) {
                    state = 42;
                }
                else if(']' == c) {
                    state = 43;
                }
                else if('(' == c) {
                    state = 44;
                }
                else if(')' == c) {
                    state = 45;
                }
                else if(is_Delim(c)) {
                    state = 13;
                }
                else if('+' == c) {
                    state = 15;
                }
                else if('-' == c) {
                    state = 16;
                }
                else if('*' == c) {
                    state = 17;
                }
                else if('/' == c) {
                    state = 22;
                }
                else if('<' == c) {
                    state = 23;
                }
                else if('>' == c) {
                    state = 27;
                }
                else if('=' == c) {
                    state = 31;
                }
                else if('!' == c) {
                    state = 33;
                }
                else if(':' == c) {
                    state = 35;
                }
                else {
                    //error
                }
            case 1:
                if(isAlpha(c) || isDigit(c) || '_' == c) {
                    state = 1;
                    //self loop
                }
                else {
                    state = 2;
                }
            case 2:
                ungetchar(c);
                return 
    
    c = getchar1()
    }

}

void removeComments(char *testcaseFile, char *cleanFile)
{

}