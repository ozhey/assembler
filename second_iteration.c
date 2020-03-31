#include "assembler.h"

#define SYMBOL_ADDRESS_LEFT_SHIFT 3

/*This macro removes white charecters from the string and prints an error if there's a comma*/
#define REMOVE_WHITE_ZERO_COMMA(string) {\
	if (remove_white(string) > 0) {\
		print_errors(UNECESSARY_COMMA,lc);\
		continue;\
		}\
	}

/*the function gets the symbol list and the instructions list and adds the addresses to the symbols in the instructions list.
 * if a symbol isn't found in the symbol list, an error will be printed.*/
void add_address_2symbols(symbol *symbol_head, data *instruct_head, data **extern_head);

/*the function executes the whole 2nd pass of the algorithm except print the output*/
void second_iteration(FILE *file, symbol *symbol_head, data *data_head, data *instruct_head, data **extern_head)
{
	int lc = 0; /*lc - line counter*/
	char line[LINE_SIZE], word[LINE_SIZE], symbol_name[MAX_SYMBOL_LENGTH];
	while (fgets(line,LINE_SIZE,file) != NULL) 
	{
		lc++;
		remove_white(line); /*remove white charecters and commas*/
		if (((*line) == '\0') || ((*line) == ';'))/*true - a comment line or an empty line*/
			continue;
		if (line[strlen(line) -1] == '\n')
			line[strlen(line) -1] = '\0';
		get_word(word, line);
		if (word[strlen(word) -1] == ':') /*true - a symbol definition*/
			get_word(word, line); /*skip the symbol*/
		if ((!strcmp(word, ".data")) || (!strcmp(word, ".string")) || (!strcmp(word, ".extern")))/*true - data, string or extern*/
			continue; /*these instructions were already handled in the first iteration*/
		else if (!strcmp(word, ".entry"))
		{
			REMOVE_WHITE_ZERO_COMMA(line); /*remove white charecters and checks that there are 0 commas*/
			get_word(word, line);
			is_symbol(symbol_head, word, symbol_name, lc); /*checks if it's a legal symbol. if not, errors will be printed*/
			insert_entry_sign(symbol_head, symbol_name, lc); /*validates that the symbol has been defined before or prints errors otherwise*/
			REMOVE_WHITE_ZERO_COMMA(line);
		} 	
	}
	add_address_2symbols(symbol_head, instruct_head, extern_head); /*adds the missing addresses to the data*/
}

/*the function gets the symbol list and the instructions list and adds the addresses to the symbols in the instructions list.
 * if a symbol isn't found in the symbol list, an error will be printed.*/
void add_address_2symbols(symbol *symbol_head, data *instruct_head, data **extern_head)
{
	data *instruct_ptr = instruct_head;
	symbol *sym_ptr = symbol_head;
	int ic = 0;
	while (instruct_ptr != NULL) /*the loop will update addresses for symbols that has been declared*/
	{
		if (strlen(instruct_ptr->symbol_name)) /*if there's a symbol sign, we will search for a symbol in the symbol list*/
		{
			while (sym_ptr != NULL)
			{
				if (!strcmp(instruct_ptr->symbol_name,sym_ptr->name)) /*a symbol found in the symbol list*/
				{
					if (sym_ptr->type == EXTERNAL) /*if the symbol was defined externally it will be added to the externals list*/ 
					{
						instruct_ptr->code = E; /*sets the E in ARE because its external*/
						addtolist_data(extern_head, ic + IC_START, instruct_ptr->symbol_name, instruct_ptr->code);
					}
					else /*the symbol was defined in the file*/
					{
						instruct_ptr->code = sym_ptr->address << SYMBOL_ADDRESS_LEFT_SHIFT; /*sets the code to be the address of the symbol in the symbols table*/
						instruct_ptr->code |= R; /*R because it was defined in the file*/
					}
					strcpy(instruct_ptr->symbol_name,"\0"); /*a reminder: the symbol name was used to mark symbols in the first iteration. it is no longer needed.*/
				}
				sym_ptr = sym_ptr->next;
			}
			sym_ptr = symbol_head;
		}
		instruct_ptr = instruct_ptr->next;
		ic++;
	}
	instruct_ptr = instruct_head;
	while (instruct_ptr != NULL)
	{
		if (strlen(instruct_ptr->symbol_name)) /*if theres a symbol that was used as an operand but not defined in the file*/
			print_errors(SYMBOL_NOT_DEFINED,instruct_ptr->code); /*the code contains the line of the command*/
		instruct_ptr = instruct_ptr->next;
	}
}
