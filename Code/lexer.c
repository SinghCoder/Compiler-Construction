/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/

#include "hashtable.h"
#include "lexerDef.h"
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief search string in lookup table to check if it's keyword or identifier
 * 
 * @param lexeme string to search for
 * @return token_name - ID or hash_table entry
 */
token_name search_lookup_table(char *lexeme) 
{
  int num = search_hash_table(lookup_table, lexeme);
  if (KEY_NOT_FOUND == num) {
    return ID;
  } else {
    return num;
  }
}

/**
 * @brief Get the token object recognized by the DFA
 * 
 * @return TOKEN - line_no , rnumerated value, name/int_value/real_value
 */

TOKEN get_token() 
{

  if (lexeme_begin == BUFFER_SIZE) 
  {
    lexeme_begin = 0;
  }
  TOKEN tkn;

  tkn.line_no = line_no;
  int lex_size = forward_ptr - lexeme_begin;
  if (lex_size < 0) {
    lex_size += num_of_rounds * BUFFER_SIZE;
    num_of_rounds = 0;
  }
  int last_index = (lex_size < MAX_LEXEME_LEN) ? lex_size : MAX_LEXEME_LEN - 1;
  lexeme[last_index] = '\0';

  if (2 == state) 
  {
    if (lex_size > 20) 
    {
        tkn.name = LEX_ERROR;
        tkn.str = lexeme;
        return tkn;
    }

    token_name name = search_lookup_table(lexeme);
    tkn.name = name;
    tkn.str = lexeme;
    return tkn;
  }

  if (4 == state || 6 == state) {
    tkn.name = NUM;
    tkn.num = atoi(lexeme);
  }

  if (8 == state || 12 == state) {
    tkn.name = RNUM;
    tkn.rnum = atof(lexeme);
  }
  return tkn;
}

/**
 * @brief retract and push back a character to stream
 * 
 * @param num_of_char - number of chracters to retract
 */

void retract(int num_of_char) 
{
  forward_ptr -= num_of_char;
  if (forward_ptr < 0) {
    forward_ptr += BUFFER_SIZE;
  }
  just_retracted = true;
}


/**
 * @brief Read tokens from the file and make a mapping array which maps enumerated tokens to strings
 * 
 */
void populate_terminal_string() 
{

  FILE *file = fopen("tokens.txt", "r");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *t_file = malloc(sizeof(char) * (length + 1));
  if (t_file == NULL) 
  {
      perror("terminal_string filling failed\n");
      exit(1);
  }

  fread(t_file, sizeof(char), length, file);
  t_file[length] = '\0';
  fclose(file);

  char *tk_read = NULL;
  int i;
  tk_read = strtok(t_file, ", \n");

  for (i = 0; tk_read != NULL; i++) 
  {
    strcpy(terminal_string[i], tk_read);
    tk_read = strtok(NULL, ", \n");
  }

  free(t_file);
}

/**
 * @brief inserts the entries for keywords in the lookup_table hashtable
 * 
 */

void populate_lookup_table() 
{

  hash_insert(lookup_table, "integer", INTEGER);
  hash_insert(lookup_table, "real", REAL);
  hash_insert(lookup_table, "boolean", BOOLEAN);
  hash_insert(lookup_table, "of", OF);
  hash_insert(lookup_table, "array", ARRAY);
  hash_insert(lookup_table, "start", START);
  hash_insert(lookup_table, "end", END);
  hash_insert(lookup_table, "declare", DECLARE);
  hash_insert(lookup_table, "module", MODULE);
  hash_insert(lookup_table, "driver", DRIVER);
  hash_insert(lookup_table, "program", PROGRAM);
  hash_insert(lookup_table, "get_value", GET_VALUE);
  hash_insert(lookup_table, "print", PRINT);
  hash_insert(lookup_table, "use", USE);
  hash_insert(lookup_table, "with", WITH);
  hash_insert(lookup_table, "parameters", PARAMETERS);
  hash_insert(lookup_table, "true", TRUE);
  hash_insert(lookup_table, "false", FALSE);
  hash_insert(lookup_table, "takes", TAKES);
  hash_insert(lookup_table, "input", INPUT);
  hash_insert(lookup_table, "returns", RETURNS);
  hash_insert(lookup_table, "AND", AND);
  hash_insert(lookup_table, "OR", OR);
  hash_insert(lookup_table, "for", FOR);
  hash_insert(lookup_table, "in", IN);
  hash_insert(lookup_table, "switch", SWITCH);
  hash_insert(lookup_table, "case", CASE);
  hash_insert(lookup_table, "break", BREAK);
  hash_insert(lookup_table, "default", DEFAULT);
  hash_insert(lookup_table, "while", WHILE);
}

/**
 * @brief used to fill the buffer from source code
 * 
 * @param fp - pointer to source code file
 */

void populate_buffer(FILE *fp) 
{
  int num;
  if (forward_ptr == BUFFER_SIZE) 
  {
    forward_ptr = 0;
    num_of_rounds++;
  }
  num = fread(&buffer[forward_ptr], 1, BUFFER_SIZE / 2, fp);
  if (num != BUFFER_SIZE / 2)
    buffer[num + forward_ptr] = EOF;
}

/**
 * @brief does initialization work for lexer
 * 
 * - initializes lookup table for keywords
 * - reset DFA initial state
 * - resets forward pointer and line number 
 * - populates buffer
 * 
 * @param source 
 */

void lexer_init(FILE *source) 
{
  init_hash_table(lookup_table);
  populate_terminal_string();
  populate_lookup_table();

  state = 0;
  lexeme_begin = 0;
  forward_ptr = 0;
  just_retracted = false;
  line_no = 1;
  num_of_rounds = 0;  
  int num = fseek(source, 0, SEEK_SET); // go back to start of source code file
  populate_buffer(source);
}

/**
 * @brief gives a character from buffer to dfa, refills buffer when needed
 * 
 * @param fp - pointer to source file
 * @return char 
 */
char get_char(FILE *fp) 
{
  if ((forward_ptr == BUFFER_SIZE || forward_ptr == BUFFER_SIZE / 2) &&
      just_retracted == false) {
    populate_buffer(fp);
  }
  char c = buffer[forward_ptr];
  int lex_index = forward_ptr - lexeme_begin;
  if (lex_index < 0) {
    lex_index += BUFFER_SIZE;
  }
  if(lex_index < MAX_LEXEME_LEN)
    lexeme[lex_index] = c;
  forward_ptr++;
  just_retracted = false;
  return c;
}

/**
 * @brief Get the next token object
 * 
 * @param fp - source code file pointer
 * @return TOKEN 
 */
TOKEN get_next_token(FILE *fp) {
  char c;
  TOKEN tkn;
  while (true) 
  {
    tkn.line_no = line_no;
    switch (state) {
    case 0:;
      c = get_char(fp);
      if (isalpha(c)) {
        state = 1;
      } else if (isdigit(c)) {
        state = 3;
      } else if ('.' == c) {
        state = 38;
      } else if (';' == c) {
        state = 40;
      } else if (',' == c) {
        state = 41;
      } else if ('[' == c) {
        state = 42;
      } else if (']' == c) {
        state = 43;
      } else if ('(' == c) {
        state = 44;
      } else if (')' == c) {
        state = 45;
      } else if (EOF == c) {
        tkn.name = DOLLAR;
        return tkn;
      } else if (' ' == c || '\n' == c || '\t' == c) {
        tkn.num = c;
        if ('\n' == c) {
          line_no++;
        }
        state = 13;
      } else if ('+' == c) {
        state = 15;
      } else if ('-' == c) {
        state = 16;
      } else if ('*' == c) {
        state = 17;
      } else if ('/' == c) {
        state = 22;
      } else if ('<' == c) {
        state = 23;
      } else if ('>' == c) {
        state = 27;
      } else if ('=' == c) {
        state = 31;
      } else if ('!' == c) {
        state = 33;
      } else if (':' == c) {
        state = 35;
      }

      else {
        state = 48;
      }
      break;

    case 1:;
      c = get_char(fp);
      if (isalnum(c) || '_' == c) {
        state = 1;
      } else {
        state = 2;
      }
      break;

    case 2:;
      retract(1);
      tkn = get_token();
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 3:;
      c = get_char(fp);
      if (isdigit(c)) {
        state = 3;
      } else if ('.' == c) {
        state = 5;
      } else {
        state = 4;
      }
      break;

    case 4:;
      retract(1);
      tkn = get_token();
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 5:;
      c = get_char(fp);
      if ('.' == c) {
        state = 6;
      } else if (isdigit(c)) {
        state = 7;
      } else {
        retract(1);
        state = 48;
      }
      break;

    case 6:;
      retract(2);
      tkn = get_token();
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 7:;
      c = get_char(fp);
      if (isdigit(c)) {
        state = 7;
      } else if ('e' == c || 'E' == c) {
        state = 9;
      } else {
        state = 8;
      }
      break;

    case 8:;
      retract(1);
      tkn = get_token();
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 9:;
      c = get_char(fp);
      if ('+' == c || '-' == c) {
        state = 10;
      } else if (isdigit(c)) {
        state = 11;
      } else {
        retract(1);
        state = 48;
      }
      break;

    case 10:;
      c = get_char(fp);
      if (isdigit(c)) {
        state = 11;
      } else 
      {
        retract(1);
        state = 48;
      }
      break;

    case 11:;
      c = get_char(fp);
      if (isdigit(c)) {
        state = 11;
      } else {
        state = 12;
      }
      break;

    case 12:;
      retract(1);
      tkn = get_token();
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 13:;
      c = get_char(fp);
      if (' ' == c || '\n' == c || '\t' == c) {
        if ('\n' == c)
          line_no++;
        state = 13;
      } else {
        state = 14;
      }
      break;

    case 14:;
      retract(1);
      tkn.name = DELIM;
      lexeme_begin = forward_ptr;
      state = 0;
      break;

    case 15:;
      tkn.name = PLUS;
      tkn.str = "+";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 16:;
      tkn.name = MINUS;
      tkn.str = "-";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 17:;
      c = get_char(fp);
      if ('*' == c) {
        state = 19;
      } else {
        state = 18;
      }
      break;

    case 18:;
      retract(1);
      tkn.name = MUL;
      tkn.str = "*";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 19:;
      lexeme_begin++;
      c = get_char(fp);
      if ('*' != c) {
        state = 19;
        if('\n' == c)
          line_no++;
      } else {
        state = 20;
      }
      break;

    case 20:;
      lexeme_begin++;
      c = get_char(fp);
      if ('*' == c) {
        state = 21;
      } else {
        state = 19;
        if('\n' == c)
          line_no++;
      }
      break;

    case 21:;
      state = 0;
      // get_char(fp);
      lexeme_begin = forward_ptr;
      break;

    case 22:;
      tkn.name = DIV;
      tkn.str = "/";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 23:;
      c = get_char(fp);
      if ('=' == c) {
        state = 25;
      } else if ('<' == c) {
        state = 26;
      } else {
        state = 24;
      }
      break;

    case 24:;
      retract(1);
      tkn.name = LT;
      tkn.str = "<";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 25:;
      tkn.name = LE;
      tkn.str = "<=";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 26:;
      c = get_char(fp);
      if ('<' == c) {
        state = 46;
      } else {
        retract(1);
        tkn.name = DEF;
        tkn.str = "<<";
        lexeme_begin = forward_ptr;
        state = 0;
        return tkn;
      }
      break;

    case 27:;
      c = get_char(fp);
      if ('=' == c) {
        state = 29;
      } else if ('>' == c) {
        state = 30;
      } else {
        state = 28;
      }
      break;

    case 28:;
      retract(1);
      tkn.name = GT;
      tkn.str = ">";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 29:;
      tkn.name = GE;
      tkn.str = ">=";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 30:;
      c = get_char(fp);
      if ('>' == c) {
        state = 47;
      } else {
        retract(1);
        tkn.name = ENDDEF;
        tkn.str = ">>";
        lexeme_begin = forward_ptr;
        state = 0;
        return tkn;
      }
      break;

    case 31:;
      c = get_char(fp);
      if ('=' == c) {
        state = 32;
      } else {
        retract(1);
        state = 48;
      }
      break;

    case 32:;
      tkn.name = EQ;
      tkn.str = "==";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 33:;
      c = get_char(fp);
      if ('=' == c) {
        state = 34;
      } else {
        retract(1);
        state = 48;
      }
      break;

    case 34:;
      tkn.name = NE;
      tkn.str = "!=";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 35:;
      c = get_char(fp);
      if ('=' == c) {
        state = 36;
      } else {
        state = 37;
      }
      break;

    case 36:;
      tkn.name = ASSIGNOP;
      tkn.str = ":=";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 37:;
      retract(1);
      tkn.name = COLON;
      tkn.str = ":";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 38:;
      c = get_char(fp);
      if ('.' == c) {
        state = 39;
      } else 
      {
        retract(1);
        state = 48;
      }
      break;

    case 39:;
      tkn.name = RANGEOP;
      tkn.str = "..";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 40:;
      tkn.name = SEMICOL;
      tkn.str = ";";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 41:;
      tkn.name = COMMA;
      tkn.str = ",";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 42:;
      tkn.name = SQBO;
      tkn.str = "[";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 43:;
      tkn.name = SQBC;
      tkn.str = "]";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 44:;
      tkn.name = BO;
      tkn.str = "(";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;

    case 45:;
      tkn.name = BC;
      tkn.str = ")";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;
    case 46:;
      tkn.name = DRIVERDEF;
      tkn.str = "<<<";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;
    case 47:;
      tkn.name = DRIVERENDDEF;
      tkn.str = ">>>";
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
      break;
    case 48:
      tkn.name = LEX_ERROR;
      int lex_size = forward_ptr - lexeme_begin;
      if (lex_size < 0) {
        lex_size += num_of_rounds * BUFFER_SIZE;
        num_of_rounds = 0;
      }
      int last_index = (lex_size < MAX_LEXEME_LEN) ? lex_size : MAX_LEXEME_LEN - 1;
      lexeme[last_index] = '\0';
      tkn.str = lexeme;
      lexeme_begin = forward_ptr;
      state = 0;
      return tkn;
    default:;
      break;
    }
  }
  return tkn;
}

/**
 * @brief Printing all the lexemes recognized by the DFA
 * 
 * @param source - source file pointer
 */

void tokenize_source_file(FILE *source)
{
  TOKEN tkn;

  printf("%-15s  |  %-20s  |  %-20s\n", "LINE_NUMBER", "LEXEME", "TOKEN_NAME");

  if (source == NULL) {
    printf("source null : print token stream\n");
  }

  while (true) {
    tkn = get_next_token(source);
    if (tkn.name == DOLLAR) 
    {
      break;
    } 
    else 
    {
      if (tkn.name == LEX_ERROR) 
      {
        printf("==========================================================\n");
        printf("%-15d  |  %-20s  |  %-20s\n", tkn.line_no, tkn.str,
               "LEXICAL ERROR");
        printf("==========================================================\n");
      } 
      else 
      {
        if (tkn.name != DELIM) 
        {
          printf("%-15d  |  ", tkn.line_no);
          switch (tkn.name) {
          case NUM:
            printf("%-20d  |  ", tkn.num);
            break;
          case RNUM:
            printf("%-20f  |  ", tkn.rnum);
            break;
          default:
            printf("%-20s  |  ", tkn.str);
            break;
          }
          printf("%-20s\n", terminal_string[tkn.name]);
        }
      }
    }
  } // end of while
}

/**
 * @brief Remove comments from the source file
 * 
 * @param source - source file pointer
 * @param no_comment_file - destination file name
 */
void remove_comments(FILE *source, char *no_comment_file) {

  FILE *outp_fptr = fopen(no_comment_file, "w");
  int state = 0;
  char ch = get_char(source);

  while (ch != EOF) {
    switch (state) 
    {
      case 0:
      {
        if ('*' == ch)
          state = 1;
        else
          fputc(ch, outp_fptr);
        break;
      }
      case 1:
      {
        if ('*' == ch)
          state = 2;
        else 
        {
          state = 0;
          fputc('*', outp_fptr);
          fputc(ch, outp_fptr);
        }
        break;
      }
      case 2:
      {
        if ('*' == ch)
          state = 3;
        else 
        {
          state = 2;
          if ('\n' == ch)
            fputc(ch, outp_fptr);
        }
        break;
      }
      case 3:
      {
        if ('*' == ch)
          state = 0;
        else
          state = 2;
        break;
      }
      default:
        break;
    }
    ch = get_char(source);
  } // end of while - file read

  fclose(outp_fptr);

  printf("Comments removed!! Do you want to check the output : (Y/N)\n");
  char choice;
  scanf("\n%c", &choice);

  if (choice == 'Y') 
  {
      FILE *fptr = fopen(no_comment_file, "r");
      char * line = NULL;
      size_t len = 0;
      ssize_t read;
      
      if (fptr == NULL)
      {
        printf("Unable to write to file %s\n", no_comment_file);
        return;
      }

      while ((read = getline(&line, &len, fptr)) != -1) 
      {
          printf("%s", line);
      }
      if (line)
          free(line);

      getchar();
      printf("\nPress any character to continue\n");
      char ch2;
      scanf("%c", &ch2);
      fclose(fptr);
  }
}
