#include "assembler.h"

#define ASSEMBLER_FILE ".as"

char *assembly_language_words [] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop","r0","r1","r2","r3","r4","r5","r6","r7",".data",".string",".entry",".extern","data","string","entry","extern"};

int main(int argc, char *argv[])
{
	char file_name[MAX_FILE_NAME] = "";
	FILE *file;
	symbol *symbol_head = NULL; /*a list to contain the symbols list*/
	data *data_head = NULL; /*a list to contain all of the data section*/
	data *instruct_head = NULL; /*a list to contain all of the instructions section*/
	data *extern_head = NULL; /*a list to contain the external variables*/
	int i, ic = 0, dc = 0; /*ic - instructions counter, dc - data counter*/
	if (argc == 1)
		print_errors(NO_COMMAND_LINE_VARIABLES);
	for (i = 1; i < argc; i++) 
	{
		strcpy(file_name,argv[i]); 
		strcat(file_name,ASSEMBLER_FILE);
		if (!(file = fopen(file_name,"r"))) 
		{
			print_errors(OPEN_FILE_FAILURE);
			continue;
		}
		printf("File opened: %s\n",file_name);
		first_iteration(file, &symbol_head, &data_head, &instruct_head, &ic, &dc);
		rewind(file); 
		if (print_errors(CHECK_ERRORS) == FALSE) /*if no errors were found in the first iteration, the program will continue to the second iteration*/
			second_iteration(file, symbol_head, data_head, instruct_head, &extern_head);
		if (print_errors(CHECK_ERRORS) == FALSE) /*no errors were found so far, no memory overflow*/ 
		{
			printf("File proccessed successfuly\n");
			output(symbol_head, data_head, instruct_head, extern_head, file_name, ic, dc);
		}
		ic = 0;
		dc = 0;
		print_errors(RESET_ERRORS); /*reset the errors flag*/
		free_data(data_head); free_data(instruct_head); free_data(extern_head); free_symbol(symbol_head); /*free all of the lists*/
		data_head = NULL; symbol_head = NULL; extern_head = NULL; instruct_head = NULL; /*reset the head pointers to NULL*/
		fclose(file);
	}
	return 0;
}
