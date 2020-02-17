#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexerDef.h"

void lex_error(int line_no, char *errStr)
{
    printf("%d ) Lexical Error : %s\n", line_no, errStr);
}

void init()
{
    state = 0;
    c = '$';
    id_count = 0;
	line_count = 0;
	lex_len = 0;
	la_str[1] = '\0';
	forward_ptr = -1;
	get_stream(fp);
    //buffer[BUFFER_SIZE]
}

//Initialize/Refill buffer
void get_stream(FILE *fp)
{
   // return fp;
    int num;
    
	//Single buffer treated as two buffers.
	//Forward pointer = -1 at the beginning
	if( -1 == forward_ptr)
	{
		forward_ptr = 0;
    	num = fread(&buffer[0], 1, BUFFER_SIZE, fp);
	}
	//Second buffer filled up
	if( BUFFER_SIZE == forward_ptr)
	{
        forward_ptr = 0;
    	num = fread(&buffer[forward_ptr + BUFFER_SIZE/2], 1, BUFFER_SIZE/2, fp);
	}
	//First buffer filled up
	else if( BUFFER_SIZE/2 == forward_ptr)
	{
		num = fread(&buffer[0], 1, BUFFER_SIZE/2, fp);
	}
    //If not enough characters left to read, append EOF
	if(BUFFER_SIZE/2 != num)
	{
        buffer[num+forward_ptr] = EOF;  
	}
}

void removeComments(FILE *ifp, FILE* ofp)
{
	state = 0;
	char x = get_char(ifp);
	switch(state)
	{
		case 0: if('*' == x) {
			
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

/*
	look for atoi() and atof()
	EOF == c? case 0: state 46
	while ( (c= getchar()) != EOF) me c should be int
*/

void add_char(char next_char) { 
	if(lex_len <= MAX_LEXEME_LENGTH) 
	{
		lexeme[lex_len] = next_char;
		lex_len++;
	}
	else 
	{
		lex_error(line_count + 1, "Identifier length exceeds 20");
	}
}

char get_char(FILE* fp) {
	/*if(){	// if buffer is read completely
		getStream(fp);
    }*/
	//c = fgetc(fp);
	//forward_ptr++;
	////add_char(c);
	if((forward_ptr == BUFFER_SIZE || forward_ptr == BUFFER_SIZE/2))	// && just_retracted == false
	{
        get_stream(fp);
    }
    char c = buffer[forward_ptr];
    int lex_index = forward_ptr - lexeme_begin;
    if(lex_index<0)
	{
        lex_index += BUFFER_SIZE;
    }
    lexeme[lex_index] = c; 
    forward_ptr++;
    //printf("%c %d", c, forward_ptr-1);
    // just_retracted = false;
    //return c;
	return c;
}

bool is_alphabet (char c)
{
	if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool is_digit (char c)
{
	if(c >= '0' && c <= '9')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool is_delim (char c)
{
	if( (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') )
	{
		return true;
	}
	else
	{
		return false;
	}
}


TOKEN getNextToken()
{
	TOKEN t;
	//use the feof instead o
	while(true){
	//for identifer and num
	la_str[0] = c;
	//perror(la_str);
		switch(state){
			case 0:
				{
					c = get_char(fp);
					if(is_alphabet(c)) 
					{
						//add_char(c);
						id_count++;
						state = 1;
					}
					else if( is_digit(c) ) 
					{
						//add_char(c);
						state = 3;
					}
					else if( is_delim(c) )
					{
						if('\n' == c)
						{
							line_count++;
						}
						state = 13;
					}
					else if( '.' == c)
					{
						state = 38;
					}
					else if( ';' == c )
					{
						state = 40;
					}
					else if( ',' == c )
					{
						state = 41;
					}
					else if( '['  == c)
					{
						state = 42;
					}
					else if( ']' == c )
					{
						state = 43;
					}
					else if( '(' == c )
					{
						state = 44;
					}
					else if( ')' == c )
					{
						state = 45;
					}
					else if( '+' == c )
					{
						state = 15;
					}
					else if( '-' == c )
					{
						state = 16;
					}
					else if( '*' == c )
					{
						state = 17;
					}
					else if( '/' == c )
					{
						state = 22;
					}
					else if( '<' == c )
					{
						state = 23;
					}
					else if( '>' == c )
					{
						state = 27;
					}
					else if( '=' == c )
					{
						state = 31;
					}
					else if( '!' == c )
					{
						state = 33;
					}
					else if( ':' == c )
					{
						state = 35;
					}
					else if( EOF == c)
					{
						state = 46;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat("Invalid character read : ", la_str);
					}
					c = get_char(fp);
				}
				break;

			case 1:
				{
					if( (is_alphabet(c) || is_digit(c) || ('_' == c) ) && id_count<20) 
					{
						//add_char(c);
						c = get_char(fp);
						id_count++;
						state = 1;
					}
					else
					{
						//add_char('\0');
						state = 2;
					}
				}
				break;
			case 2: 
				{
					//la_str[0] = c;
					//perror(la_str);
					ungetc(c, fp);
					//lookup till end_ptr-1
					retract(1);
					lexeme_begin = forward_ptr;
					printf("ID (%s)",lexeme);	// or KW
					lex_len = 0;
					id_count = 0;
					state = 0;
				}
				break;
			case 3: 
				{
					if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 3;
					}
					else if('.' == c)
					{
						//add_char(c);
						c = get_char(fp);
						state = 5;
					}
					else
					{
						//add_char('\0');
						state = 4;
					}
				}
				break;
			case 4: 
				{
					ungetc(c, fp);
					// do atoi() to get value  till end_ptr-1
					retract(1);
					lexeme_begin = forward_ptr;
					printf("NUM(%d) ", atoi(lexeme));
					lex_len = 0;
					state = 0;
				}
				break;
			case 5:
				{
					if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 7;
					}
					else if( '.' == c)
					{
						state = 6;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat("Expected a digit after ., found : ", la_str);
					}
				}
				break;
			case 6:
				{
					ungetc('.', fp);
					ungetc('.', fp);
					// atoi( ( f_ptr .. end_ptr - 2 )
					lex_len--;	//remove one . which was added
					//add_char('\0');
					retract(2);
					lexeme_begin = forward_ptr;
					printf("NUM(%d) ", atoi(lexeme));
					lex_len = 0;
					state = 0;
				}
			case 7: 
				{
					if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 7;
					}
					else if('E' == c || 'e' == c)
					{
						//add_char(c);
						c = getc(fp);
						state = 9;
					}
					else
					{
						//add_char('\0');
						state = 8;
					} 
				}
				break;
			case 8: 
				{
					ungetc(c, fp);
					// do atof()  till end_ptr-1
					lexeme[forward_ptr-1] = '\0';
					printf("RNUM(%f) ", atof(lexeme));
					state=0;
				}
				break;
			case 9: 
				{
					if('+' == c || '-' == c)
					{
						//add_char(c);
						c = get_char(fp);
						state = 10;
					}
					else if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 11;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat( "Expected +/-/digit after e/E, found :", la_str);
						// invalidate the token read
					}
				}
					break;
			case 10: 
				{
					if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 11;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat( "Expected digit after +/-, found :", la_str);
						// invalidate token read
					}
				}
				break;
			case 11: 
				{
					if(is_digit(c))
					{
						//add_char(c);
						c = get_char(fp);
						state = 11;
					}
					else 
					{
						//add_char('\0');
						state = 12;
					}
				}
				break;
			case 12: 
				{
					ungetc(c,fp);
					//atof() end_ptr-1
					retract(1);
					printf("RNUM(%f) ", atof(lexeme));
					lex_len = 0;
					state = 0;
				}
				break;
			case 13:
				{
					if ( is_delim(c) )
					{
						if('\n' == c)
						{
							line_count++;
						}
						c = get_char(fp);
						state = 13;
					}
					else
					{
						state = 14;
					}
				}
				break;
			case 14:
				{
					retract(1);
					ungetc(c, fp);
					state = 0;
				}
				break;
			case 15:
				{
					//arith_op , PLUS
					lexeme[forward_ptr] = '\0';
					printf("PLUS ");
					state = 0;
				}
				break;
			case 16:
				{
					//arith_op , MINUS
					lexeme[forward_ptr] = '\0';
					printf("MINUS ");
					state = 0;
				}
				break;
			case 17:
				{
					if( '*' == c)
					{
						c = get_char(fp);
						state = 19;
					}
					else
					{
						state = 18;
					}
				}
				break;
			case 18:
				{
					ungetc(c, fp);
					//arith_op, mul
					retract(1);
					printf("MUL ");
					state = 0;
				}
				break;
			case 19:
				{	
					if( '*' == c)
					{
						c = get_char(fp);
						state = 20;
					}
					else
					{
						state = 19;
					}
				}
				break;
			case 20:
				{
					if( '*' == c)
					{
						state = 21;
					}
					else
					{
						c = get_char(fp);
						state = 19;
					}
				}
				break;
			case 21:
				{
					printf("COMMENT ");	//ignore this token
					state = 0;
				}
				break;
			case 22:
				{
					lexeme[forward_ptr] = '\0';
					printf("DIV ");
					state = 0;
				}
				break;
			case 23:
				{
					if('=' == c)
					{
						state = 25;
					}
					else if('<' == c)
					{
						state = 26;
					}
					else
					{
						state = 24;
					}
				}
				break;
			case 24:
				{
					ungetc(c, fp);
					lexeme[forward_ptr - 1] = '\0';
					printf("LT ");
				}
				break;
			case 25:
				{
					lexeme[forward_ptr] = '\0';
					printf("LE ");
					state = 0;
				}
				break;
			case 26:
				{
					lexeme[forward_ptr] = '\0';
					printf("DEF ");
					state = 0;
				}
				break;
			case 27:
				{
					if('=' == c)
					{
						state = 29;
					}
					else if('>' == c)
					{
						state = 30;
					}
					else
					{
						state = 28;
					}
				}
				break;
			case 28:
				{
					lexeme[forward_ptr - 1] = '\0';
					ungetc(c, fp);
					printf("GT ");
				}
				break;
			case 29:
				{
					lexeme[forward_ptr] = '\0';
					printf("GE ");
					state = 0;
				}
				break;
			case 30:
				{
					lexeme[forward_ptr] = '\0';
					printf("ENDDEF ");
					state = 0;
				}
				break;
			case 31:
				{	
					if('=' == c)
					{
						state = 32;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat( "Expected = after =, found :", la_str );
					}
				}
				break;
			case 32:
				{
					lexeme[forward_ptr] = '\0';
					printf("EQ ");
					state = 0;
				}
				break;
			case 33:
				{	
					if('=' == c)
					{
						state = 34;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat( "Expected = after !, found :", la_str );
					}
				}
				break;
			case 34:
				{
					lexeme[forward_ptr] = '\0';
					printf("NE ");
					state = 0;
				}
				break;
			case 35:
				{
					if('=' == c)
					{
						state = 36;
					}
					else
					{
						state = 37;
					}
				}
				break;
			case 36:
				{
					lexeme[forward_ptr] = '\0';
					printf("ASSIGNOP ");
					state = 0;
				}
				break;
			case 37:
				{
					ungetc( c, fp );
					retract(1);
					printf("COLON ");
					state = 0;
				}
				break;
			case 38:
				{
					if( '.' == c)
					{
						state = 39;
					}
					else
					{
						state = TRAP_STATE;
						la_str[0] = c;
						err_msg = strcat( "Expected . after ., found :", la_str );
					}
				}
				break;
			case 39:
				{
					retract(1);
					printf("RANGEOP ");
					state = 0;
				}
				break;
			case 40:
				{
					retract(1);
					ungetc(c, fp);
					printf("SEMICOL ");
					state = 0;
				}
				break;
			case 41:
				{
					retract(1);
					ungetc(c, fp);
					printf("COMMA ");
					state = 0;
				}
				break;
			case 42:
				{
					retract(1);
					ungetc(c, fp);
					printf("SQBO ");
					state = 0;
				}
				break;
			case 43:
				{
					retract(1);
					ungetc(c, fp);
					printf("SQBC ");
					state = 0;
				}
				break;
			case 44:
				{
					retract(1);
					ungetc(c, fp);
					printf("BO ");
					state = 0;
				}
				break;
			case 45:
				{
					retract(1);
					ungetc(c, fp);
					printf("BC ");
					state = 0;
				}
				break;
			case 46:
				{
					// ungetc(c, fp);
					printf("LEXING DONE... ");
					state = 0;
					// exit(0);
				}
				break;
			case TRAP_STATE:
			default:
				{
					// ungetc() the character read
					ungetc(c, fp);
					lex_len = 0;	//ignore whatever u read rn
					lex_error(line_count + 1, err_msg); //line_no, msg
					state = 0;
				}
		}	//  End of switch
	}	// End of while
	/*
	if(BUFFER_SIZE == lexeme_begin)
	{
        lexeme_begin = 0;
    }
    TOKEN t;

    lex_len = forward_ptr - lexeme_begin;

    if(lex_len <0)
	{
        lex_len += BUFFER_SIZE;
    }
    lexeme[lex_len] = '\0';
	*/
}

/*
	do getc()  in each non-accepting state
	ungetc() in each retracting state
	and nothin in (accepting + non-retracting) states
*/

int main ( int argc, char *argv[] ) {
	FILE* fp;
	fp = fopen(argv[1], "r");
	if(fp == NULL){
		printf("FILE OPEN ERROR\n");
		exit(1);
	}
	init();
//	#if 0
	FILE *source = fopen("input.txt", "r");
	TOKEN t;
	while(1){
        t = getNextToken(source);
        if(t.name == END_OF_FILE){
            break;
        }
        else{
            if(t.name == LEX_ERROR){
                lexError("Error...");
            }
            else{
                if(t.name != DELIM){
                    printf("%s ",terminal_string[ t.name ]); 
                }
                else{
                    if(t.num == '\n'){
                        printf("\n");
                    }
                }
            }        
        }
    }
// #endif
}	// End of main