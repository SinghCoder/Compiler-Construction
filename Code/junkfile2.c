#include <stdio.h>
#include <ctype.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "lexerDef.h"

void lexError(char *errStr, int line_no)
{
    printf("%d ) Lexical Error : %s\n", line_no, errStr);
token_name searchLookupTable(char* lexeme){
    char *lookupTable[30][2];
    return ID;


}

TOKEN getToken(){

    if(lexeme_begin == BUFFER_SIZE){
        lexeme_begin = 0;
    }
    TOKEN t;

    int lex_size = forward_ptr - lexeme_begin;
    if(lex_size <0){
        lex_size+= BUFFER_SIZE;
    }
    lexeme[lex_size] = '\0';

    if(2 == state){

        if(lex_size >20){
            t.name = LEX_ERROR;
            return t;    
        }

        token_name name = searchLookupTable(lexeme);
        t.name = name;
        t.str = lexeme;
        return t;
    }    

    if(4 == state || 6 == state){
        t.name = NUM;
        t.num = atoi(lexeme);
    }    

    if( 8 == state || 12 == state){
        t.name = RNUM;
        t.rnum = atof(lexeme);
    }

    return t;        
}

void retract(int num){
    forward_ptr -= num;
    if(forward_ptr < 0){
        forward_ptr += BUFFER_SIZE;
    }
    just_retracted = true;
}

void init()
{
    state = 0;
    buffer[BUFFER_SIZE]
    lexeme_begin = forward_ptr= 0;
    just_retracted = false;
    line_no = 1;
}

FILE *getStream(FILE *fp)
void getStream(FILE *fp)
{
    return fp;
    //printf("stream: ");
    //for( int i=0; i<BUFFER_SIZE; i++)
    //    printf("%c", buffer[i]);

    //printf(" nice ");    
    int num;
    if(forward_ptr == BUFFER_SIZE)
        forward_ptr = 0;
    num = fread(&buffer[forward_ptr], 1, BUFFER_SIZE/2, fp);
    if(num!=BUFFER_SIZE/2)
        buffer[num+forward_ptr] = EOF;   
        lex_index += BUFFER_SIZE;
    }
    lexeme[lex_index] = c; 
    forward_ptr++;
    //printf("%c %d", c, forward_ptr-1);
    just_retracted = false;
    return c;
}

TOKEN getNextToken()
TOKEN getNextToken(FILE* fp)
{
    char c = getchar1();
    while(c != EOF) {
        switch(state): {
            case 0: 
                if(isAlpha(c)) {
    char c;
    TOKEN t;
    while(1) {
        //printf("%d ", state);
        switch(state){
            case 0:  ; 
                c = getChar(fp);
                if(isalpha(c)) {
                    state = 1;
                }
                else if(isDigit(c)) {
                else if(isdigit(c)) {
                        state = 3;
                }
                else if('.' == c) {
@@ -51,7 +134,15 @@ TOKEN getNextToken()
                else if(')' == c) {
                    state = 45;
                }
                else if(is_Delim(c)) {
                else if(EOF == c){
                    t.name = END_OF_FILE;
                    return t;
                }
                else if(' ' == c || '\n' == c || '\t' == c) {
                    t.num = c;
                    if('\n' == c){
                        line_no++;
                    }
                    state = 13;
                }
                else if('+' == c) {
@@ -81,27 +172,415 @@ TOKEN getNextToken()
                else if(':' == c) {
                    state = 35;
                }

                else {
                    //error
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
            case 1:
                if(isAlpha(c) || isDigit(c) || '_' == c) {
                break;

            case 1:  ;
                c = getChar(fp);
                if(isalnum(c)|| '_' == c) {
                    state = 1;
                    //self loop
                }
                else {
                    state = 2;
                }
            case 2:
                ungetchar(c);
                return 
                break;

            case 2:  ;
                retract(1);
                //printf("%c %d ", buffer[forward_ptr], forward_ptr);
                t = getToken();
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 3:  ;
                c = getChar(fp);
                if(isdigit(c)){
                    state = 3;
                }
                else if('.' == c){
                    state = 5;
                }
                else{
                    state = 4;
                }
                break;

            case 4:  ;
                retract(1);
                t = getToken();
                lexeme_begin = forward_ptr; state =0; 
                return t;
                break;

            case 5:  ;
                c = getChar(fp);
                if('.' == c){
                    state = 6;
                }    
                else if(isdigit(c)){
                    state = 7;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break;

            case 6:  ;
                retract(2);
                t = getToken();
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 7:  ;
                c = getChar(fp);
                if(isdigit(c)){
                    state = 7;
                }
                else if('e' == c || 'E' == c){
                    state = 9;
                }
                else{
                    state = 8;    
                }
                break;

            case 8:  ;
                retract(1);
                t = getToken();
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 9:  ;
                c = getChar(fp);
                if('+' == c || '-' == c){
                    state = 10;
                }    
                else if(isdigit(c)){
                    state = 11;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }   
                break;

            case 10:  ;
                c = getChar(fp);
                if(isdigit(c)){
                    state = 11;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break; 

            case 11:  ;
                c = getChar(fp);
                if(isdigit(c)){
                    state = 11;
                }
                else{
                    state = 12;
                }     
                break;            

            case 12:  ;
                retract(1);
                t = getToken();
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 13:  ;
                c = getChar(fp);
                //printf("delim1 %c", c );
                if(' ' == c || '\n' == c|| '\t' == c){
                    line_no++;
                    state = 13;
                }    
                else {
                    state = 14;        
                }
                break;

            case 14:  ;
                retract(1);
                t.name = DELIM;
                //t.num = buffer[forward_ptr-1];
                lexeme_begin = forward_ptr; state =0;
                //printf("delim %c", buffer[forward_ptr] );
                return t;
                break;

            case 15:  ;
                t.name = PLUS;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 16:  ;
                t.name = MINUS;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break; 

            case 17:  ;
                c = getChar(fp);
                if('*' == c){
                    state = 19;
                }
                else{
                    state = 18;
                }     
                break;         

            case 18:  ;
                retract(1);
                t.name = MUL;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 19:  ;
                c = getChar(fp);
                if('*' != c){
                    state = 19;
                }
                else{
                    state = 20;
                }
                break;

            case 20:  ;
                c = getChar(fp);
                if('*' == c){
                    state = 21;
                }              
                else{
                    state = 19;
                }
                break;

            case 21:  ;
                state = 0;
                break;

            case 22:  ;
                t.name = DIV;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 23:  ;
                c = getChar(fp);
                if('=' == c){
                    state = 25;
                }
                else if('<' == c){
                    state = 26;
                }
                else{
                    state = 24;
                }
                break;

            case 24:  ;
                retract(1);
                t.name = LT;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 25:  ;
                t.name = LE;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 26:  ;
                t.name = DEF;
                lexeme_begin = forward_ptr; state =0; 
                return t;
                break;  

            case 27:  ;
                c = getChar(fp);
                if('=' == c){
                    state = 29;
                }
                else if('>' == c){
                    state = 30;
                }
                else{
                    state = 28;
                }
                break;

            case 28:  ;
                retract(1);
                t.name = GT;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 29:  ;
                t.name = GE;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 30:  ;
                t.name = ENDDEF;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break; 

            case 31:  ;
                c = getChar(fp);
                if('=' == c){
                    state = 32;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break;

            case 32:  ;
                t.name = EQ;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;  

            case 33:  ;
                c = getChar(fp);
                 if('=' == c){
                    state = 34;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break;

            case 34:  ;
                t.name = NE;
                lexeme_begin = forward_ptr; state =0;
                return t; 
                break;   

            case 35:  ;
                c = getChar(fp);
                if('=' == c){
                    state = 36;
                }
                else{
                    state = 37;
                }
                break;

            case 36:  ;
                t.name = ASSIGNOP;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 37:  ;
                retract(1);
                t.name = COLON;
                lexeme_begin = forward_ptr; state =0;
                //printf("colon %c %d", buffer[forward_ptr], forward_ptr);
                return t;
                break;

            case 38: ;
                c = getChar(fp);
                if('.' == c){
                    state = 39;
                }
                else{
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break;

            case 39: ;        
                t.name = RANGEOP;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 40:  ;  
                t.name = SEMICOL;
                lexeme_begin = forward_ptr; state =0; 
                return t;
                break;

            case 41: ;   
                t.name = COMMA;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 42:  ;  
                t.name = SQBO;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 43: ;   
                t.name = SQBC;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 44: ;   
                t.name = BO;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 45: ;    
                t.name = BC;
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;    

            default:  ;
                t.name = LEX_ERROR;
                //printf("%d", state);
                return t;
        }        


    c = getchar1()
    }

    return t;
}

void removeComments(FILE *ip, FILE *op){
    while(!feof(ip)){
        c = getchar(fp);
        
    }
}
void lexError(char *errStr,FILE* fp)
{

}
    printf("%d ) Lexical Error : %s\n", line_no, errStr);
    char c;
    while(1){
        if(getChar(fp) == '\n'){
            line_no++;
            break;
        }
    }
}

void removeComments(FILE *ifp, FILE* ofp)
{
	state = 0;
	int x = get_char(ifp);
    while(x!=EOF){
    	switch(state)
    	{
    		case 0: 
                if('*' == x)
                  state = 1;
                else
                    fputc(c, ofp);
                break;
            case 1:
                if('*' == x)
                    state = 2;
                else
                {
                    state = 0;
                    fputc(x, ofp)
                }
                break;
            case 2:
                if('*' == x)
                    state = 3;
                else {} //nothing
                break;
            case 3:
                if('*' == x)
                    state = 0;
                else
                    state = 2;
                break;
        }
        x = get_char(ifp);
	}        
}

	while(c != EOF)
	{
		if(!incomment && '*' != c) {
			putchar(c, ofp)
		}
		if('*' == c && 0 == onestar) 
		{
			onestar = 1;
		}
		else if('*' == c && 1 == onestar) 
		{
			twostar = 1;
		}
	}
}
