#include "assembler.h"

#define OBJECT_FILE_END ".ob"
#define EXTERNAL_FILE_END ".ext"
#define ENTRY_FILE_END ".ent"
#define MAX_BITS 32767

/*the function gets the symbol list and pointers to flags the mark if there are externals/entries in the list.
 *the function changes the flags to TRUE if external/entry symbols are found*/
void does_exist_entry(symbol *head, char *ent_flag);

/*this function gets 3 lists that represent the whole data needed after procession in the first and second operation.
 * the function then writes the information contained in them to 3 files according to the task's instructions*/
void output(symbol *symbol_head, data *data_head, data *instruct_head, data *extern_head, char *file_name, int ic, int dc) 
{
	symbol *sym_ptr = symbol_head;
	data *data_ptr = data_head;
	data *instruct_ptr = instruct_head;
	FILE *file; 
	int counter = IC_START;
	char ent_flag = FALSE;
	(*strrchr(file_name, '.')) = '\0';
	strcat(file_name, OBJECT_FILE_END);
	if ((file = fopen(file_name, "w")) == NULL)
	{
		print_errors(CREATE_FILE_FAILED);
		return;
	}
	fprintf(file, "%d %d\n", ic, dc);
	while (instruct_ptr != NULL) /*print the instructions to ob file*/
	{
		instruct_ptr -> code &= MAX_BITS; /*turns off irrelevant bits, every bit after the 14th*/
		fprintf(file, "%04d %05o\n", counter, instruct_ptr -> code);
		instruct_ptr = instruct_ptr->next;
		counter++;
	}
	while (data_ptr != NULL) /*print the data to ob file*/
	{
		data_ptr -> code &= MAX_BITS; /*turns off irrelevant bits*/
		fprintf(file, "%04d %05o\n", counter, data_ptr -> code);
		data_ptr = data_ptr -> next;
		counter++;
	}
	fclose(file);
	if (extern_head != NULL) /*externals file will be created only if there are external variables in the list*/
	{
		(*strrchr(file_name, '.')) = '\0';
		strcat(file_name, EXTERNAL_FILE_END);
		if ((file = fopen(file_name, "w")) == NULL) /*create the externals file*/
		{
			print_errors(CREATE_FILE_FAILED);
			return;
		}
		while (extern_head != NULL)
		{
			fprintf(file, "%s %04d\n", extern_head -> symbol_name, extern_head -> code);
			extern_head = extern_head -> next;
		}
		fclose(file);
	}
	does_exist_entry(symbol_head, &ent_flag); /*the function checks if there are entries in the list and changes ent_flag to TRUE if there are*/
	if (ent_flag == TRUE) /*entrys file will be created only if there are entries in the list*/
	{
		(*strrchr(file_name, '.')) = '\0';
		strcat(file_name, ENTRY_FILE_END);
		if ((file = fopen(file_name, "w")) == NULL) /*create the entry file*/
		{
			print_errors(CREATE_FILE_FAILED);
			return;
		}
		while (sym_ptr != NULL) /*prints the entries*/
		{
			if (sym_ptr -> type == ENTRY)
				fprintf(file, "%s %04d\n", sym_ptr -> name, sym_ptr -> address);
			sym_ptr = sym_ptr -> next;
		}
		fclose(file);
	}
	
		
}


/*the function gets the symbol list and pointers to flags the mark if there are entries in the list.
 *the function changes the flags to TRUE if entry symbols are found*/
void does_exist_entry(symbol *head, char *ent_flag)
{
	while (head != NULL)
	{
		if (head->type == ENTRY)
			*ent_flag = TRUE;
		head = head->next;
	}
}
