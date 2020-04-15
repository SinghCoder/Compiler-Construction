/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/

#include "driver.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "treeADT.h"
#include "intermed_codegen.h"
#include "semantic_analyzer.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void store_error(int line_num, int type, char *message){	
	char *type_err;
	
	if(line_num == -1)
	{
		printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", message);
		return;
	}

	switch(type)
	{
		case LEXICAL_ERROR:
			type_err = "LEXICAL ERROR";
			snprintf(lexical_errors[lex_err_num++], MAX_ERROR_STRING_LENGTH, ANSI_COLOR_RED "%d : %s | " ANSI_COLOR_RESET "%s\n",line_num, type_err, message);
		break;
	
		case SYNTACTIC_ERROR:
			type_err = "SYNTACTIC ERROR";
			snprintf(syntactic_errors[syn_err_num++], MAX_ERROR_STRING_LENGTH, ANSI_COLOR_RED "%d : %s | " ANSI_COLOR_RESET "%s\n",line_num, type_err, message);
		break;
	
		case SEMANTIC_ERROR:
			type_err = "SEMANTIC ERROR";
			snprintf(semantic_errors[sem_err_num++], MAX_ERROR_STRING_LENGTH, ANSI_COLOR_RED "%d : %s | " ANSI_COLOR_RESET "%s\n",line_num, type_err, message);
		break;

		case INTERNAL_ERROR:
			type_err = "INTERNAL ERROR";
		break;
	}	
}

void print_errors()
{
	if(lex_err_num != 0)
	{
		printf("\t\tLEXICAL ERRORS :\n");
		for(int i = 0; i < lex_err_num; i++)
		{
			printf("%s\n", lexical_errors[i]);
		}
		return;
	}

	if(syn_err_num != 0)
	{
		printf("\t\tSYMANTIC ERRORS :\n");
		for(int i = 0; i < syn_err_num; i++)
		{
			printf("%s\n", syntactic_errors[i]);
		}
		return;
	}

	if(sem_err_num != 0)
	{
		printf("\t\tSEMANTIC ERRORS :\n");
		for(int i = 0; i < sem_err_num; i++)
		{
			printf("%s\n", semantic_errors[i]);
		}
		return;
	}
}

/**
 * @brief Prints menu options
 *
 */
void print_menu() {
	printf("LEVEL 4: Symbol table/type Checking/Semantic rules modules work/ handled static and dynamic arrays in type checking and code generation\n");
	printf("\t\t Press the option for the defined task\n");
	printf("-----------------------------------------------------------------\n");
	printf("0. Exit\n");
	printf("1. Print tokens generated by lexer\n");
	printf("2. Print parse tree for the program\n");
	printf("3. Print abstract syntax tree for the program\n");
	printf("4. Memory used\n");
	printf("5. Symbol table\n");
	printf("6. Activation record size\n");
	printf("7. Static and dynamic arrays\n");
	printf("8. Error reporting and total compiling time\n");
	printf("9. Code generation\n");
	printf("-----------------------------------------------------------------\n");
	printf("Enter your choice:  ");
}

/**
 * @brief Read tokens from the file and make a mapping array which maps
 * enumerated tokens to strings
 *
 */
void populate_terminal_string() {

	FILE *file = fopen("tokens.txt", "r");
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *t_file = malloc(sizeof(char) * (length + 1));
	if (t_file == NULL) {
		perror("terminal_string filling failed\n");
		exit(1);
	}

	fread(t_file, sizeof(char), length, file);
	t_file[length] = '\0';
	fclose(file);

	char *tk_read = NULL;
	int i;
	tk_read = strtok(t_file, ", \n");

	for (i = 0; tk_read != NULL; i++) {
		strcpy(terminal_string[i], tk_read);
		tk_read = strtok(NULL, ", \n");
	}

	free(t_file);
}

/**
 * @brief inserts all non-terminal strings in an array, which is used for
 * mapping enumerated non-terminals to corresponding string values
 *
 */
void populate_non_terminal_string() {
	FILE *file = fopen("non_terminals.txt", "r");

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *nt_file = malloc(sizeof(char) * (length + 1));

	if (nt_file == NULL) {
		store_error(-1, INTERNAL_ERROR, "Parser init failed");
		exit(1);
	}

	fread(nt_file, sizeof(char), length, file);
	nt_file[length] = '\0';
	fclose(file);

	char *nt_read = NULL;
	int i;
	nt_read = strtok(nt_file, ", \n");

	for (i = 0; nt_read != NULL; i++) {
		strcpy(non_terminal_string[i], nt_read);
		nt_read = strtok(NULL, ", \n");
	}
	free(nt_file);
}

int main(int argc, char *argv[]) {

	if (argc < 3) {
		store_error(-1, SYNTACTIC_ERROR, "Run as ./a.out test_case asm_file");
		exit(1);
	}

	setvbuf(stdout, NULL, _IONBF, 0);

	populate_non_terminal_string();
	populate_terminal_string();

	int choice;
	char source_file[100];
	strcpy(source_file, argv[1]);
	strcpy(assembly_file, argv[2]);

	FILE *source = fopen(source_file, "r");
	if (source == NULL) {
		printf("Error opening file\n");
	}

	while (true) {
		print_menu();
		scanf("%d", &choice);
		printf("\n");

		switch (choice) {
			case 0:
			{
				exit(0);
			}
			break;

			case 1:
			{
				lexer_init(source);
				tokenize_source_file(source);
			} 
			break;

			case 2: 
			{
				lexer_init(source);
				parser_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if(fptr == NULL) 
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL) 
				{
					printf("Empty parse tree\n");
				}
				
				print_parse_tree(ptr);
				free_grammar();
				fclose(fptr);

			} 
			break;
			
			case 3:
			{
				lexer_init(source);
				parser_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL)
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL)
				{
					printf("Empty parse tree\n");
				}
				curr_start = 0;
				curr_end = 0;

				tree_node *ast_tree = construct_ast(ptr);

				construct_symtable(ast_tree);
				second_ast_pass(ast_tree);			

				print_parse_tree(ast_tree);
				
				free_grammar();
				fclose(fptr);
			}
			break;
			
			case 4: 
			{

				lexer_init(source);
				parser_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if(fptr == NULL) 
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();

				num_tree_nodes = 0;
				tree_node *ptr = parse_input_source_code(source);
				int num_pt_nodes = num_tree_nodes;

				num_tree_nodes = 0;
				fclose(fptr);      

				tree_node *ast_tree = construct_ast(ptr);
				// ptr->visited = false;
				traverse_ast(ast_tree);
				int num_ast_nodes = num_tree_nodes;

				printf("Parse tree number of nodes = %d\n", num_pt_nodes);
				
				unsigned long mem_pt = num_pt_nodes * sizeof(tree_node);
				printf("Allocated memory = %lu\n", mem_pt);
				
				printf("AST number of nodes = %d\n", num_ast_nodes);
				
				unsigned long mem_ast = num_ast_nodes * sizeof(tree_node);
				printf("Allocated memory = %lu\n", mem_ast);
				
				double comp_perc = ((mem_pt - mem_ast + 0.0)/(mem_pt + 0.0)) * 100;
				printf("Compression percentage = %lf\n", comp_perc);
			} 
			break;
			
			case 5: 
			{
				lexer_init(source);
				parser_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL)
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL)
				{
					printf("Empty parse tree\n");
				}

				tree_node *ast_tree = construct_ast(ptr);
				construct_symtable(ast_tree);        
				second_ast_pass(ast_tree);			

				print_symbol_table(ast_tree->scope_sym_tab);

				free_grammar();
				fclose(fptr);
			}
			break;
			
			case 6:
			{
				lexer_init(source);
				parser_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL)
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL)
				{
					printf("Empty parse tree\n");
				}

				tree_node *ast_tree = construct_ast(ptr);
				construct_symtable(ast_tree);        
				second_ast_pass(ast_tree);

				print_activation_records();
			}
			break;

			case 7:
			{
				lexer_init(source);
				parser_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL)
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL)
				{
					printf("Empty parse tree\n");
				}

				tree_node *ast_tree = construct_ast(ptr);				
				construct_symtable(ast_tree);        
				second_ast_pass(ast_tree);

				print_arrays(root_sym_tab_ptr);
			}
			break;

			case 8:
			{
				clock_t    start_time, end_time;

                double total_CPU_time, total_CPU_time_in_seconds;

                start_time = clock();
				
				lex_err_num = 0;
				syn_err_num = 0;
				sem_err_num = 0;

				lexer_init(source);
				parser_init();
				intermed_codegen_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL)
				{
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL)
				{
					printf("Empty parse tree\n");
				}
				
				if(lex_err_num != 0 || syn_err_num != 0){
					print_errors();
					break;
				}

				tree_node *ast_tree = construct_ast(ptr);
				construct_symtable(ast_tree);        
				second_ast_pass(ast_tree);

				print_errors();

				generate_ir(ast_tree);	
				// print_quadruples();
				// print_symbol_table(root_sym_tab_ptr);	

				assembly_file_ptr = fopen(assembly_file, "w");

				generate_code();

				fclose(assembly_file_ptr);

				printf("Code compiles successfully..........\n\n");

				free_grammar();
				fclose(fptr);

				end_time = clock();

                total_CPU_time  =  (double) (end_time - start_time);

                total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;

				printf("Total CPU time : %lf\n", total_CPU_time);
				printf("Total CPU time(in seconds) : %lf\n", total_CPU_time_in_seconds);
			}
			break;

			case 9: 
			{
				lex_err_num = 0;
				syn_err_num = 0;
				sem_err_num = 0;

				lexer_init(source);
				parser_init();
				intermed_codegen_init();
				semantic_analyzer_init();

				FILE *fptr = fopen("grammar.txt", "r");
				if (fptr == NULL) {
					perror("fopen");
				}
				grammar_fill(fptr);

				populate_first_sets();

				populate_follow_sets();

				create_parse_table();
				// print_parse_table();

				tree_node *ptr = parse_input_source_code(source);

				if (ptr == NULL) {
					printf("Empty parse tree\n");
				}
				tree_node *ast_tree = construct_ast(ptr);
				// parse_tree_file_ptr = fopen("pt.txt", "w");
				// print_parse_tree_for_tool(ast_tree, ast);
				// fclose(parse_tree_file_ptr);

				if(lex_err_num || syn_err_num){
					print_errors();
					printf("Please firstly correct the errors in the code\n\n");
					break;
				}

				construct_symtable(ast_tree);
				second_ast_pass(ast_tree);

				if(sem_err_num){
					print_errors();
					printf("Please firstly correct the errors in the code\n\n");
					break;
				}

				generate_ir(ast_tree);	
				// print_quadruples();
				// print_symbol_table(root_sym_tab_ptr);	

				assembly_file_ptr = fopen(assembly_file, "w");

				generate_code();

				fclose(assembly_file_ptr);

				printf("Code compiles successfully..........\n\n");

				free_grammar();
				fclose(fptr);
			}
			break;
			
			default: 
			{
				exit(0);
			}
			break;
		} // end of switch
	}
	fclose(source);
} // end of main
