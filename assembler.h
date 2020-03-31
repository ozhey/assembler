#ifndef assembler
#define assembler 

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdarg.h>

#define FALSE 0
#define TRUE 1 
#define BASE 10
#define IC_START 100
#define DC_START 0
#define MAX_FILE_NAME 30 
#define MAX_WORD_NUM 16383
#define MIN_WORD_NUM -16384
#define BITS_NUMBER 15
#define LINE_SIZE 82 /*80 letters + '\n' + '\0'*/
#define MAX_SYMBOL_LENGTH 32 /*31 letters + '\0'*/
#define START_MEMORY_ADDRESS 100 /*the beginning of the memory addres*/
#define NUM_OF_WORDS_IN_ASSEMBLY_LANGUAGE 32 /*the number of saved words in the language*/
#define NUM_OF_OPCODE_IN_ASSEMBLY_LANGUAGE 16 /*the number of opcodes in the language*/
#define NUM_OF_REGISTERS_ASSEMBLY_LANGUAGE 8 /*the number of opcodes in the language*/
#define A 4
#define R 2
#define E 1

extern char *assembly_language_words[];/*an array of all the assembly language words*/

enum {CHECK_ERRORS, NO_COMMAND_LINE_VARIABLES, LINE_TOO_LONG,MEMORY_OVERFLOW, OPEN_FILE_FAILURE,UNECESSARY_COMMA, TOO_MANY_COMMAS,MISSING_COMMA, SYMBOL_TOO_LONG,\
	  ILLEGAL_SYMBOL_NAME_SAVED_WORD, ILLEGAL_SYMBOL_NAME_NOT_ALPHA, ILLEGAL_CHARACTERS_IN_SYMBOL_NAME, DUPLICATE_SYMBOL, NOT_A_NUMBER, NUMBER_OUT_OF_RANGE,
	  ILLEGAL_STRING_FORMAT, UNDEFINED_COMMAND, EXCESSIVE_INPUT, ILLEGAL_ADDRESSING_METHOD,ENTRY_NOT_DEFINED,SYMBOL_NOT_DEFINED, CREATE_FILE_FAILED, RESET_ERRORS}; /*errors flags*/
	  
enum {INSTANT_ADDRESSING, DIRECT_ADDRESSING, INDIRECT_REGISTER , DIRECT_REGISTER, NO_OPERAND}; /*addressing methods*/

typedef enum {EXTERNAL,ENTRY,UNDEFINED} Type; /*a symbol can be external, entry, or undefined - such as command lines or data*/

typedef enum {CODE,DATA,UNDEF} Location; /*the symbol can be placed at the code or data section or only be used for the output files entry or extern (not in the code itself)*/

typedef struct symbol { 
	char name[MAX_SYMBOL_LENGTH];
	int address;
	Location location;
	Type type;
	struct symbol *next;
} symbol;

typedef struct data {
	short code;
	char symbol_name[MAX_SYMBOL_LENGTH];
	struct data *next;
} data;

/*@@_______print_errors.c functions_______@@*/
/*This function get a flag integer number and print the error message acording to the value*/
int print_errors(int flag, ...);


/*@@_______auxiliary_func.c functions_______@@*/
/*this function removes white charecters and commas from the begginning of a string
 *the functions also returns the number of commas found*/
int remove_white(char *line);
/*This function trims the next word from the line and place it into the word variable.
The function also rids of white charecters after the first word*/ 
void get_word(char *word, char *line);
/*this function get a string array, a word, the array begin index and the array end index and  check whether the word is in the array between the indexs (return TRUE (1)) or not in the array between the indexs (return FALSE (0))*/
int is_appears(char ** array, char * word, int beg_index, int end_index);
/*this function gets an integer that represents the number of commas found and prints an error if it's not 1
 * the function returns true if the number is 1 or false otherwise */
int one_comma(int comma, int lc);
 
/*@@_______first_iteration.c functions_______@@*/
/*The functions gets a file and builds 3 linked lists 
 * The linked lists represent the symbols table, the data table and the 
 * instructions table as they are supposeded to be after the first iteration*/
void first_iteration(FILE *file, symbol **symbol_head, data **data_head, data **instruct_head,int *ic, int *dc);

/*@@_______second_iteration.c functions_______@@*/
/*the function executes the whole 2nd pass of the algorithm except print the output*/
void second_iteration(FILE *file, symbol *symbol_head, data *data_head, data *instruct_head, data **extern_head);

/*@@_______output.c functions_______@@*/
/*this function gets 3 lists that represent the whole data needed after procession in the first and second operation.
 * the function then writes the information contained in them to 3 files according to the task's instructions*/
void output(symbol *symbol_head, data *data_head, data *instruct_head, data *extern_head, char *file_name, int ic, int dc);

/*@@_______symbol_list.c functions_______@@*/
/*The function adds a new node to a symbol linked list*/
void addtolist_symbol(symbol **head, char *name, int address, Location location, Type type, int line_number);
/*this function get the string word, if the word is a valid symbol name, trims the ':' at the end and copy it to the symbol name.
 * if the word is not a legal symbol name, the function will print the error*/
void is_symbol(symbol *head, char *word, char *symbol_name, int lc);
/*this function check whether the symbol name is in the linked list (and return TRUE (1)) or not in the linked list (and return FALSE (0))*/
void check_duplicate_symbol(symbol *head,char *symbol_name, Type type, int lc);
/*the function gets the symbol table and the ic and updates the address of all of the data symbols*/
void set_address_data_symbols(symbol *head, int ic);
/*the function gets a symbol (.entry symbol) and checks that it was defined before. if it wasn't, errors will be printed*/
void insert_entry_sign(symbol *head, char *symbol_name, int lc);
/*free the list*/
void free_symbol(symbol *head);

/*@@_______data_list.c functions_______@@*/
/*The function adds a new node to a data linked list*/
void addtolist_data(data **head, short word, char *symbol_name, int line_number);
/*The function gets a line that consists numbers and white chars or commas and adds the numbers to the data list.
 * the function prints errors if found.*/
void add_data_2data(data **head, char *line, char *symbol_name, int lc, int *dc);
/*the function gets a string and adds it's contents to the data list if it's a legal string*/
void add_string_2data(data **head, char *line, char *symbol_name, int lc, int *dc);
/*the function gets a command and 2 operands. if it's legal, the functions adds the codes to the instructions list.
 * if it's not legal, the function will print adequate errors.*/
void add_instruction_2data(data **instruct_head, symbol *symbol_head, char *command, char *source_op, char *target_op, int *ic, int lc);
/*free the list*/
void free_data(data *head);

#endif
