#include "assembler.h"

/*The function adds a new node to a symbol linked list*/
void addtolist_symbol(symbol **head, char *name, int address, Location location, Type type, int line_number)
{
	symbol *p = *head;
	symbol *new = (symbol *)malloc(sizeof(symbol));
	if(new == NULL) 
		print_errors(MEMORY_OVERFLOW,line_number);
	new->address = address;
	new->type = type;
	new->location = location;
	strcpy(new->name,name);
	if (*head == NULL)
	{
		*head = new;
		(*head)->next = NULL;
		return;
	}
	while (p->next != NULL)
		p = p->next;
	p->next = new;
	new->next = NULL;
}

/*this function check whether the symbol name is in the linked list (and return TRUE (1)) or not in the linked list (and return FALSE (0))*/
void check_duplicate_symbol(symbol *head, char *symbol_name, Type type, int lc)
{
	symbol *ptr = head;
	while (ptr != NULL)
	{
		if (type == EXTERNAL && (!strcmp(ptr->name, symbol_name) && (ptr->type == UNDEFINED))) /*we want to define an external symbol. we must check that it wasn't defined before as a normal symbol*/
				print_errors(DUPLICATE_SYMBOL, lc);
		else if (type == UNDEFINED && (!strcmp(ptr->name, symbol_name) && (ptr->type != ENTRY))) /*we want to define a normal symbol. we must check that it wasn't defined before at all (unless it was entry)*/
				print_errors(DUPLICATE_SYMBOL, lc);
		ptr = ptr->next;
	}
}

/*this function get the string word, if the word is a valid symbol name, trims the ':' at the end and copy it to the symbol name.
 * if the word is not a legal symbol name, the function will print the error*/
void is_symbol(symbol *head, char *word, char *symbol_name, int lc)
{
	char *ptr = word;
	if (is_appears(assembly_language_words, word, 0, NUM_OF_WORDS_IN_ASSEMBLY_LANGUAGE))/*true - the symbol name equal to a saved word in the assembly language*/
	{
		print_errors(ILLEGAL_SYMBOL_NAME_SAVED_WORD, lc);
		return;
	}
	if (!((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')))
	{
		print_errors(ILLEGAL_SYMBOL_NAME_NOT_ALPHA, lc);
		return;
	}
	while ((*ptr) != '\0')
	{
		if (!((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || (*ptr >= '0' && *ptr <= '9'))) 
		{
			print_errors(ILLEGAL_CHARACTERS_IN_SYMBOL_NAME, lc, (*ptr));
			return;
		}
		ptr++;
	}
	if ((strlen(word)) >= MAX_SYMBOL_LENGTH)/*symbol name length is over MAX_SYMBOL_LENGTH -1 (31 characters)*/
	{
		print_errors(SYMBOL_TOO_LONG,lc);
		return;
	}
	strncpy(symbol_name, word,MAX_SYMBOL_LENGTH);
}

/*the function gets an entry symbol and adds the entry sign to all of the symbols in the list with the same name*/
void insert_entry_sign(symbol *head, char *symbol_name, int lc)
{
	symbol *ptr = head;
	char flag = FALSE;
	while (ptr != NULL)
	{
		if (!strcmp(ptr->name,symbol_name) && (ptr->type != EXTERNAL)) /*found a symbol with the same name that has been defined in the file.*/
		{
			flag = TRUE;
			ptr->type = ENTRY;
		}
		ptr = ptr->next;
	}
	if (flag == FALSE) /*wasnt defined*/
		print_errors(ENTRY_NOT_DEFINED,lc);
}

/*the function gets the symbol table and the ic and updates the address of all of the data symbols*/
void set_address_data_symbols(symbol *head, int ic)
{
	while (head != NULL)
	{
		if (head->location == DATA)
			head->address += (ic + IC_START);
		head = head->next;
	}
}

/*free the list*/
void free_symbol(symbol *head)
{
	symbol *next;
	while (head != NULL) {
		next = head->next;
		free(head);
		head = next;
	}
}
