#include "assembler.h"

#define MAX_INSTANT_NUM 2047
#define MIN_INSTANT_NUM -2048
#define A 4
#define R 2
#define E 1
#define OPCODE_LEFT_SHIFT 11
#define SOURCE_ADD_METHOD_LEFT_SHIFT 7
#define TARGET_ADD_METHOD_LEFT_SHIFT 3
#define INSTANT_ADDRESSING_LEFT_SHIFT 3
#define SOURCE_REG_INDEX 6
#define TARGET_REG_INDEX 3
#define LAST_BIT_LOCATION 14

/*The function adds a new node to a data linked list*/
void addtolist_data(data **head, short code, char *symbol_name, int line_number)
{
	data *p = *head;
	data *new = (data *)malloc(sizeof(data));
	if(new == NULL)
		print_errors(MEMORY_OVERFLOW,line_number);
	new->code = code;
	strcpy(new->symbol_name,symbol_name);
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

/*The function gets a line that consists numbers and white chars or commas and adds the numbers to the data list.
 * the function prints errors if found.*/
void add_data_2data(data **head, char *line, char *symbol_name, int lc,  int *dc)
{
	int comma = 1, number;
	char word[LINE_SIZE], *endptr;
	while (*line)
	{
		if (one_comma(comma,lc) == FALSE)
			return;
		get_word(word,line);
		number = strtol(word,&endptr,BASE); /*if number is not a valid number (*endp) != \0) */
		if (*endptr) /*if endptr contains anything it means that it wasn't a legal number*/
		{ 
			print_errors(NOT_A_NUMBER, lc);
			return;
		}
		else if ((number > MAX_WORD_NUM) || (number < MIN_WORD_NUM)) /*true - vlue out of range*/
		{ 
			print_errors(NUMBER_OUT_OF_RANGE, lc);
			return;
		}
		if (number < 0)
			number |= 1 << (BITS_NUMBER-1);
		addtolist_data(head,number,symbol_name,lc);
		comma = remove_white(line);
		(*dc)++;
	}
	if (comma > 0)
		print_errors(UNECESSARY_COMMA,lc);
}

/*the function gets a string and adds it's contents to the data list if it's a legal string*/
void add_string_2data(data **head, char *line, char *symbol_name, int lc, int *dc)
{
	int i;
	char *ptr = (line+1);
	ptr = strrchr(line,'\"'); /*if another apostrophes aren't found in the line, NULL will be returned*/
	if (line[0] != '\"' || ptr == NULL) /*ptr == NULL means that there's no closing apostrophes*/
	{
		print_errors(ILLEGAL_STRING_FORMAT,lc);
		return;
	}
	*ptr = '\0'; /*closes the string with \0*/
	for (i = 1; i <= strlen(line); i++) /*adds the string contents to the data list*/
	{
		addtolist_data(head,line[i],symbol_name,lc);
		(*dc)++;
	}
	strcpy(line,ptr+1);
}

/*the function gets an operand and returns it's addressing method. if an operand is illegal, errors will be printed*/
int addressing_method(char *operand, symbol *symbol_head, int lc)
{
	char symbol_flag[MAX_SYMBOL_LENGTH] = "\0", *endptr;
	int number = strtol((operand+1), &endptr, BASE);
	if (strlen(operand) == 0) /*if the operand is just \0, it means that no operand was entered*/
		return NO_OPERAND;
	else if (operand[0] == '#' && strlen(endptr) == 0) /*if the first char is # and the rest of the string is a valid number, the operand is a number*/
	{
		if (number >= MAX_INSTANT_NUM || number <= MIN_INSTANT_NUM)
			print_errors(NUMBER_OUT_OF_RANGE, lc);
		return INSTANT_ADDRESSING;
	}
	else if (operand[0] == '*' && (is_appears(assembly_language_words, (operand+1), NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE, NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE + NUM_OF_REGISTERS_ASSEMBLY_LANGUAGE)))
			return INDIRECT_REGISTER; /*if the first char is * and the rest of the string is one of the registers, then it's an indirect register method*/
	else if (is_appears(assembly_language_words, operand, NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE, NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE + NUM_OF_REGISTERS_ASSEMBLY_LANGUAGE)) /*if operand is a register*/
		return DIRECT_REGISTER; /*if the word is one of the registers*/
	else /*it has to be a symbol. otherwise, it's an error*/
	{
		is_symbol(symbol_head, operand, symbol_flag, lc); /*if it's not a legal symbol, errors will be printed*/
		return DIRECT_ADDRESSING;	
	}
}

/*the function gets a command and the operands's methods and prints errors if the combination is not legal*/
void legal_command_line(char *command, int source_method, int target_method, int lc)
{
	if (!strcmp(command,"mov") || !strcmp(command,"sub") || !strcmp(command,"add"))
	{
		if (source_method == NO_OPERAND || target_method == INSTANT_ADDRESSING || target_method == NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else if (!strcmp(command,"lea"))
	{
		if (source_method != DIRECT_ADDRESSING || target_method == INSTANT_ADDRESSING || source_method == NO_OPERAND || target_method == NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else if (!strcmp(command,"rts") || !strcmp(command,"stop"))
	{
		if (source_method != NO_OPERAND || target_method != NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else if (!strcmp(command,"jmp") || !strcmp(command,"bne") || !strcmp(command,"jsr"))
	{
		if (source_method != NO_OPERAND || (target_method != DIRECT_REGISTER && target_method != DIRECT_ADDRESSING))
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else if (!strcmp(command,"prn"))
	{
		if (source_method != NO_OPERAND || target_method == NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else if (!strcmp(command,"cmp"))
	{
		if (source_method == NO_OPERAND || target_method == NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
	else /*clr, not, inc, red, dec*/
	{
		if (source_method != NO_OPERAND || target_method == INSTANT_ADDRESSING || target_method == NO_OPERAND)
			print_errors(ILLEGAL_ADDRESSING_METHOD,lc);
	}
}

/*the function calculates and returns the number of memory words needed for the command*/
int calculate_how_many_words(int source_method, int target_method)
{
	if (source_method == NO_OPERAND && target_method == NO_OPERAND)
		return 1;
	else if (source_method == NO_OPERAND || target_method == NO_OPERAND)
		return 2;
	else if ((source_method == DIRECT_REGISTER || source_method == INDIRECT_REGISTER) && (target_method == DIRECT_REGISTER || target_method == INDIRECT_REGISTER))
		return 2;
	else 
		return 3;
}

/*this function returns the first memory word built from a command, source and target method*/
short first_memory_word(char *command, int source_method, int target_method)
{
	short i, msk = 1, first_word = 0;
	for (i = 0; i < NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE; i++)
		if (!strcmp(command,assembly_language_words[i])) /*if the opcode was found*/
			first_word = i << OPCODE_LEFT_SHIFT; 
	if (source_method != NO_OPERAND) /*if theres a source operand, the method will be inserted to the first memory word*/
		first_word |= (msk << (SOURCE_ADD_METHOD_LEFT_SHIFT + source_method));
	if (target_method != NO_OPERAND) /*if theres a target operand, the method will be inserted to the first memory word*/
		first_word |= (msk << (TARGET_ADD_METHOD_LEFT_SHIFT + target_method));
	first_word |= A;
	return first_word;
}

/*this function gets an operand and returns a memory word according to the bits rules
 * if the operand is a register, the function needs to get the amount of bits to left shift to place the register correctly (source\target)*/
short operand_to_memory_word(char *operand, int method, int reg_left_shift, int lc)
{
	short msk = 0, memory_word = 0;
	if (method == INSTANT_ADDRESSING)
	{
		msk = strtol((operand+1), NULL, BASE); /*the first char is '#' so we will convert from the second char*/
		if (msk < 0) /*if its a negative number we will turn on the last bit*/
			memory_word |= 1 << LAST_BIT_LOCATION;
		memory_word |= msk << INSTANT_ADDRESSING_LEFT_SHIFT;
		memory_word |= A;
		
	}
	else if (method == INDIRECT_REGISTER)
	{
		msk = strtol((operand+2), NULL, BASE); /*the first charecters are '*r' so we will convert the number placed 2 charecters afterwards*/
		memory_word |= msk << reg_left_shift;
		memory_word |= A;
	}
	else if (method == DIRECT_REGISTER) 
	{
		msk = strtol((operand+1), NULL, BASE); /*the first charecter is 'r' so we will convert the number placed 1 charecter afterwards*/
		memory_word |= msk << reg_left_shift;
		memory_word |= A;
	}
	else /*it's a symbol*/
		memory_word = lc; /*we will save the line for usage in the second iteration.*/
	return memory_word;
}

short second_memory_word(char *source_op, char *target_op, int source_method, int target_method, int lc)
{
	short second_word = 0;
	char *ptr = source_op;
	short msk;
	if ((source_method == DIRECT_REGISTER || source_method == INDIRECT_REGISTER) && (target_method == DIRECT_REGISTER || target_method == INDIRECT_REGISTER))
	{ /*if there are 2 registers (direct or indirect) the function will put both of them in the second memory word*/
		if ((ptr++)[0] == '*') /*if it's a pointer to a register we will point to the number only*/
			ptr++;
		msk = strtol(ptr, NULL, 10);
		second_word |= msk << SOURCE_REG_INDEX; /*put the source register in the word*/
		ptr = target_op;
		if ((ptr++)[0] == '*') /*if it's a pointer to a register we will point to the number only*/
			ptr++;
		msk = strtol(ptr, NULL, BASE);
		second_word |= msk << TARGET_REG_INDEX; /*put the target register in the word*/
		second_word |= A;
	}
	else /*if it's not 2 registers, we will handle only one operand and put it in the 2nd memory word*/
	{
		if (strlen(source_op) == 0) /*if there's just one operand, we will handle the target op*/
			second_word = operand_to_memory_word(target_op,target_method, TARGET_REG_INDEX, lc);
		else /*if there are 2, we will handle the source op*/
			second_word = operand_to_memory_word(source_op,source_method, SOURCE_REG_INDEX, lc);
	}
	return second_word;
}


/*the function gets a command and 2 operands. if it's legal, the functions adds the codes to the instructions list.
 * if it's not legal, the function will print adequate errors.*/
void add_instruction_2data(data **instruct_head, symbol *symbol_head, char *command, char *source_op, char *target_op, int *ic, int lc)
{
	int source_method = addressing_method(source_op,symbol_head,lc); /*checks what's the addressing method of the source operand*/
	int target_method = addressing_method(target_op,symbol_head,lc); /*checks what's the addressing method of the target operand*/
	short first_code = 0, second_code = 0, third_code = 0, L = 0;
	legal_command_line(command,source_method,target_method,lc);
	L = calculate_how_many_words(source_method,target_method); /*pust in L the number of memory words required*/
	first_code = first_memory_word(command, source_method, target_method); /*gets the first memory word*/
	addtolist_data(instruct_head,first_code,"\0",lc);
	if (L > 1) /*if there's a 2nd memory word*/
	{
		second_code = second_memory_word(source_op ,target_op, source_method, target_method, lc);
		if (source_method == NO_OPERAND && target_method == DIRECT_ADDRESSING) /*if there's just one operand (target) and it's a symbol*/
			addtolist_data(instruct_head,second_code,target_op,lc);
		else if (source_method == DIRECT_ADDRESSING) /*there are 2 symbols and the source operand is a symbol*/
			addtolist_data(instruct_head,second_code,source_op,lc);
		else
			addtolist_data(instruct_head,second_code,"\0",lc); /*there's no symbol*/
	}
	if (L == 3) /*if there's a 3rd memory word */
	{
		third_code = operand_to_memory_word(target_op, target_method, TARGET_REG_INDEX, lc);
		if (target_method == DIRECT_ADDRESSING) /*if the source operand is a symbol, we want to mark it in the data list*/
			addtolist_data(instruct_head,third_code,target_op,lc);
		else /*if it's not a symbol, the symbol_name in the data list will be empty*/
			addtolist_data(instruct_head,third_code,"\0",lc);
	}
	*ic += L;
}

/*free the list*/
void free_data(data *head)
{
	data *next;
	while (head != NULL) {
		next = head->next;
		free(head);
		head = next;
	}
}
