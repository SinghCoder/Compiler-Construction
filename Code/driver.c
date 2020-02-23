#include "lexer.h"
#include "parser.h"
#include "treeADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void remove_comments(char *source_file, char *no_comment_file)
{
	FILE *inp_fptr;
	FILE *outp_fptr;

	inp_fptr = fopen(source_file, "r");
	outp_fptr = fopen(no_comment_file, "w");

	int state = 0;
	int x = fgetc(inp_fptr);
	while (x != EOF) {
		switch (state) {
			case 0:
				if ('*' == x)
					state = 1;
				else
					fputc(x, outp_fptr);
				break;
			case 1:
				if ('*' == x)
					state = 2;
				else
				{
					state = 0;
					fputc('*', outp_fptr);
					fputc(x, outp_fptr);
				}
				break;
			case 2:
				if ('*' == x)
					state = 3;
				else {} //nothing
				break;
			case 3:
				if ('*' == x)
					state = 0;
				else
					state = 2;
				break;
			default:
				break;
		}
		x = fgetc(inp_fptr);
	}	// end of while - file read

	fclose(inp_fptr);
	fclose(outp_fptr);

	printf("Comments removed!! Do you want to check the output : (Y/N)\n");
	char ch;
	scanf("\n%c", &ch);

	if(ch == 'Y')
	{
		FILE *fptr = fopen(no_comment_file, "r");
		char buf[100];


		while(fgets(buf, 99, fptr) != NULL)
		{
			printf("%s", buf);
		}

		printf("\nPress any character to continue\n");
		char ch;
		scanf("%c", &ch);
	}
}

void print_menu()
{
	printf("\t\t MENU\n");
	printf("-----------------------------------------------------------------\n");
	printf("1. Remove comments from the source code\n");
	printf("2. Print tokens generated by lexer\n");
	printf("3. Print parse tree for the program\n");
	printf("4. Print time taken for tokenizing and parsing\n");
	printf("5. Print first sets\n");
	printf("6. Print follow sets\n");
	printf("7. Exit\n");
	printf("-----------------------------------------------------------------\n");
	printf("Enter your choice:  ");
}
int main(int argc, char *argv[]) 
{
  setvbuf(stdout, NULL, _IONBF, 0);
  // FILE *source = fopen("test.txt", "r");
  // lexer_init();
  // printf("lexer initialized %d\n", lookup_table[0].present);
  // parser_init();
  // printf("parser initialized\n");

  // getStream(source);
  // printf("stream obtained\n");
  // print_token_stream(source);

  // FILE *fptr = fopen("grammar.txt", "r");
  // if (fptr == NULL) 
  // {
  //   perror("fopen");
  // }
  // grammar_fill(fptr);

  // populate_first_sets();
    
  // populate_follow_sets();
  
  // createParseTable();
  // ull *fset = get_rule_first_set(grammar[0].head);
  // print_rule_fset(fset);

  // print_first_sets();
  // print_follow_sets();
  
  // print_grammar();
  // print_parse_table();

  //  tree_node* ptr = parseInputSourceCode(source);

  // free(source);
  // printf(".......end.......\n");

  int choice;
  char source_file[100];
  strcpy(source_file, argv[1]);

  while(true)
  {
	print_menu();
	scanf("%d", &choice);
	printf("\n");

	switch(choice)
	{
		case 1:
			{
				char no_comment_file[100];
				printf("Enter name for comment removed source code file\n");
				scanf("%s", no_comment_file);
				remove_comments(source_file, no_comment_file);
				// print_menu();
			}
			break;
		case 2:
			{
				lexer_init();
				FILE *source = fopen(source_file, "r");
				getStream(source);
				print_token_stream(source);
			}
			break;
		case 3:
			{
				lexer_init();
				parser_init();

				FILE *source = fopen(source_file, "r");
				getStream(source);

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL) 
				{
				  perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();
					
				populate_follow_sets();
				
				createParseTable();
				tree_node* ptr = parseInputSourceCode(source);
				
			}
			break;
		case 4:
			{
				clock_t    start_time, end_time;

                double total_CPU_time, total_CPU_time_in_seconds;

                start_time = clock();

                    FILE *source = fopen("test.txt", "r");
					lexer_init();
					parser_init();
					getStream(source);
					// print_token_stream(source);

					FILE *fptr = fopen("grammar.txt", "r");
					if (fptr == NULL) 
					{
					  perror("fopen");
					}
					grammar_fill(fptr);

					populate_first_sets();
						
					populate_follow_sets();
					
					reset_lexer_dfa(source);

					// fseek(source, 0, SEEK_SET);   done in reset_lexer_dfa

					createParseTable();
					// ull *fset = get_rule_first_set(grammar[0].head);
					// print_parse_table();

					tree_node* ptr = parseInputSourceCode(source);

					free(source);

                end_time = clock();

                total_CPU_time  =  (double) (end_time - start_time);

                total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;

				printf("Total CPU TIME taken : %lf secs\nTotal CPU time in seconds %lf \n", total_CPU_time, total_CPU_time_in_seconds);
			}
			break;
		case 5:
			{
				lexer_init();
				parser_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL) 
				{
				  perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				print_first_sets();
			}
			break;
		case 6:
			{
				lexer_init();
				parser_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL) 
				{
				  perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();
					
				populate_follow_sets();
				print_follow_sets();
			}
			break;
		case 7:
		default:
			{
				// break;
				exit(1);
			}
			break;
	}
  }
} // end of main