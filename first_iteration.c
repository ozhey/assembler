#include "assembler.h"

/*This macro removes white charecters from the string and prints an error if there's a comma*/
#define REMOVE_WHITE_ZERO_COMMA(string) {\
	if (remove_white(string) > 0) {\
		print_errors(UNECESSARY_COMMA,lc);\
		continue;\
		}\
	}


/*The functions gets a file and builds 3 linked lists
 * The linked lists represent the symbols table, the data table and the 
 * instructions table as they are supposeded to be after the first iteration*/
void first_iteration(FILE *file, symbol **symbol_head, data **data_head, data **instruct_head, int *ic, int *dc)
{
	int lc = 0, comma; /*ic - instructions counter, dc - data counter, lc - line counter*/
	char line[LINE_SIZE+1] = "", word[LINE_SIZE] = "", excess, symbol_name[MAX_SYMBOL_LENGTH] = "", source_op[MAX_SYMBOL_LENGTH] = "", target_op[MAX_SYMBOL_LENGTH] = "";	
	while (fgets(line,LINE_SIZE+1,file) != NULL) 
	{
		lc++;
		strcpy(symbol_name,"\0"); /*reset the symbol name*/
		if (strlen(line) == LINE_SIZE)  /*checks if the line length is greater than 80*/
		{ 
			print_errors(LINE_TOO_LONG,lc);
			while ((excess = fgetc(file)) != '\n' && excess != EOF); /*rids of excess input*/
			continue;
		}
		if (line[0] == ';') /*checks if it's a comment line*/
			continue;
		if (line[strlen(line) -1] == '\n')
			line[strlen(line) -1] = '\0';
		REMOVE_WHITE_ZERO_COMMA(line);
		if (strlen(line) == 0) /*empty line*/
			continue;
		get_word(word,line);
		if (word[strlen(word) -1] == ':') /*Checks if there's a symbol*/
		{
			word[strlen(word) -1] = '\0';
			is_symbol(*symbol_head,word,symbol_name,lc); /*if the word is a legal symbol name, the function will put it in symbol_name. otherwise, it will print adequate errors*/
			REMOVE_WHITE_ZERO_COMMA(line);
			if (strlen(line) == 0) /*only symbol definition and nothing else. we will ignore the line.*/
				continue;
			get_word(word,line);
		}
		if (!strcmp(word,".data") || !strcmp(word,".string"))
		{
			if (strlen(symbol_name)) /*if there's a symbol*/
			{
				check_duplicate_symbol(*symbol_head,symbol_name,UNDEFINED,lc);
				addtolist_symbol(symbol_head, symbol_name, *dc, DATA, UNDEFINED, lc);
			}
			REMOVE_WHITE_ZERO_COMMA(line);
			if (!strcmp(word,".data")) 
				add_data_2data(data_head, line, symbol_name, lc, dc);
			else /*".string"*/
			{
				add_string_2data(data_head, line, symbol_name, lc, dc); /*adds the string to the data list*/
				REMOVE_WHITE_ZERO_COMMA(line); /*remove white charecters after the string*/
				if (strlen(line)) /*if there are any leftovers in line after the string, it means that the string wasn't legal*/
					print_errors(ILLEGAL_STRING_FORMAT,lc);
			}
			continue;
		}
		else /*if it's not data*/
		{
			if (!strcmp(word,".entry") || !strcmp(word,".extern"))
			{
				if (strlen(symbol_name)) /*If there's a symbol a warning will be printed but no error*/
					printf("Warning! A label was entered and wasn't followed by an instruction or data and therefore it is ignored.\n");
				if (!strcmp(word,".extern"))
				{
					REMOVE_WHITE_ZERO_COMMA(line);
					get_word(word, line);
					is_symbol(*symbol_head, word, symbol_name, lc);
					check_duplicate_symbol(*symbol_head,symbol_name,EXTERNAL,lc);
					addtolist_symbol(symbol_head, word, 0, UNDEF, EXTERNAL, lc);
				}
				/*".entry". will be processed in the second iteration*/
				continue;
			}
			else /*It's an instruction line OR the line contains just the symbol definition OR an undefined syntax*/
			{
				if (strlen(symbol_name))  /*if there's a symbol*/
				{
					check_duplicate_symbol(*symbol_head,symbol_name,UNDEFINED,lc);
					addtolist_symbol(symbol_head, symbol_name, (*ic) + IC_START, CODE, UNDEFINED, lc);
				}
				if (is_appears(assembly_language_words,word,0,NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE)) /*checks that the command is legal*/
				{
					REMOVE_WHITE_ZERO_COMMA(line);
					get_word(source_op,line); /*read the first operand*/
					comma = remove_white(line); /*remove white charecters before the 2nd operand*/
					get_word(target_op,line); /*read the second operand*/
					if (strlen(target_op)) /*if there are 2 operands, there must be one comma between them*/
						one_comma(comma,lc);
					else if (comma > 0) /*if there are 0 or 1 operands, there must be no commas*/
					{
						print_errors(UNECESSARY_COMMA,lc);
						continue;
					}
					REMOVE_WHITE_ZERO_COMMA(line);
					if (strlen(line)) /*if there are still charecters in the line, they are redundant, and an error will be printed*/
						print_errors(EXCESSIVE_INPUT,lc);
					if (strlen(source_op) && !strlen(target_op)) /*if there's just 1 operand, we will want to make it the target operand. the source op will be '\0'*/
					{
						strcpy(target_op,source_op);
						strcpy(source_op,"\0");
					}
					add_instruction_2data(instruct_head, *symbol_head, word, source_op, target_op, ic, lc);
				}
				else /*undefined command or just symbol declaration*/
					print_errors(UNDEFINED_COMMAND,lc);
			}
		}
	} /*end of while*/
	set_address_data_symbols(*symbol_head,*ic); /*gets the symbol table and the ic and updates the address of all of the data symbols*/

}
