#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexerDef.h"

token_name searchLookupTable(char* lexeme){
    char *lookupTable[30][2];
    return ID;
}

TOKEN getToken(){

    if(lexeme_begin == BUFFER_SIZE)
	{
        lexeme_begin = 0;
    }
    TOKEN t;

    t.line_no = line_no;
    int lex_size = forward_ptr - lexeme_begin;
    if(lex_size <0)
	{
        lex_size+= BUFFER_SIZE;
    }
    lexeme[lex_size] = '\0';
    
    if(2 == state)
	{
        
        if(lex_size >20)
		{
            t.name = LEX_ERROR;
            return t;    
        }
        
        token_name name = searchLookupTable(lexeme);
        t.name = name;
        t.str = lexeme;
        return t;
    }    

    if(4 == state || 6 == state)
	{
        t.name = NUM;
        t.num = atoi(lexeme);
    }    

    if( 8 == state || 12 == state)
	{
        t.name = RNUM;
        t.rnum = atof(lexeme);
    }

    return t;        
}

void retract(int num)
{
    forward_ptr -= num;
    if(forward_ptr < 0)
	{
        forward_ptr += BUFFER_SIZE;
    }
    just_retracted = true;
}

void init()
{
    state = 0;
    lexeme_begin = forward_ptr= 0;
    just_retracted = false;
    line_no = 1;
}

void getStream(FILE *fp)
{
    int num;
    if(forward_ptr == BUFFER_SIZE)
        forward_ptr = 0;
    num = fread(&buffer[forward_ptr], 1, BUFFER_SIZE/2, fp);
    if(num!=BUFFER_SIZE/2)
        buffer[num+forward_ptr] = EOF;   
}

char getChar(FILE *fp){
    if((forward_ptr == BUFFER_SIZE || forward_ptr == BUFFER_SIZE/2) && just_retracted == false)
	{
        getStream(fp);
    }
    char c = buffer[forward_ptr];
    int lex_index = forward_ptr - lexeme_begin;
    if(lex_index<0){
        lex_index += BUFFER_SIZE;
    }
    lexeme[lex_index] = c; 
    forward_ptr++;
    //printf("%c %d", c, forward_ptr-1);
    just_retracted = false;
    return c;
}

TOKEN getNextToken(FILE* fp)
{
    char c;
    TOKEN t;
    while(1) {
        //printf("%d ", state);
        t.line_no = line_no;
        switch(state){
            case 0:  ; 
                c = getChar(fp);
                if(isalpha(c)) {
                    state = 1;
                }
                else if(isdigit(c)) {
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
                    t.name = LEX_ERROR;
                    //printf("%d", state);
                    return t;
                }
                break;

            case 1:  ;
                c = getChar(fp);
                if(isalnum(c)|| '_' == c) {
                    state = 1;
                }
                else {
                    state = 2;
                }
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
                t.str = "+";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 16:  ;
                t.name = MINUS;
                t.str = "-";
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
                t.str = "*";
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
                t.str = "/";
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
                t.str = "<";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 25:  ;
                t.name = LE;
                t.str = "<=";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 26:  ;
                c = getChar(fp);
                if('<' == c)
                {
                    state = 46;
                }
                else
                {
                    retract(1);
                    t.name = DEF;
                    t.str = "<<";
                    lexeme_begin = forward_ptr; state =0;
                    return t;
                }
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
                t.str = ">";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 29:  ;
                t.name = GE;
                t.str = ">=";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 30:  ;
                c = getChar(fp);
                if( '>' == c)
                {
                    state = 47;
                }
                else
                {
                    retract(1);
                    t.name = ENDDEF;
                    t.str = ">>";
                    lexeme_begin = forward_ptr; state =0;
                    return t;
                }
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
                t.str = "==";
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
                t.str = "!=";
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
                t.str = ":=";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 37:  ;
                retract(1);
                t.name = COLON;
                t.str = ":";
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
                t.str = "..";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 40:  ;  
                t.name = SEMICOL;
                t.str = ";";
                lexeme_begin = forward_ptr; state =0; 
                return t;
                break;

            case 41: ;   
                t.name = COMMA;
                t.str = ",";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 42:  ;  
                t.name = SQBO;
                t.str = "[";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 43: ;   
                t.name = SQBC;
                t.str = "]";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 44: ;   
                t.name = BO;
                t.str = "(";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;

            case 45: ;    
                t.name = BC;
                t.str = ")";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;    
            case 46: ;
                t.name = DRIVERDEF;
                t.str = "<<<";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;
            case 47: ;
                t.name = DRIVERENDDEF;
                t.str = ">>>";
                lexeme_begin = forward_ptr; state =0;
                return t;
                break;
            default:  ;
                t.name = LEX_ERROR;
                //printf("%d", state);
                return t;
        }        
    
    
    }
    return t;
}

void lexError(char *errStr,FILE* fp)
{
    printf("%d ) Lexical Error : %s\n", line_no, errStr);
    // char c;
    // while(1){
    //     if(getChar(fp) == '\n'){
    //         line_no++;
    //         break;
    //     }
    // }
}

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
