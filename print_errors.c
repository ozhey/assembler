#include "assembler.h"

/*This function get a flag integer number and print the error message acording to the value*/
int print_errors(int flag, ...)
{
	static int error_flag = FALSE; /*the flag is static to make it possible to check if there were errors at any time*/
	va_list p;
	if (flag != CHECK_ERRORS) /*if theres an error, the flag will be turned on*/
		error_flag = TRUE;
	va_start(p, flag);
	switch (flag) {
	case RESET_ERRORS: /*this case resets the eror flag back to false. it should only be done after a file proccessing has finished*/
		error_flag = FALSE;
		break;
	case CHECK_ERRORS: /*in case "check errors", no action needs to be performed. the function is called just to return the errors flag value.*/
		break;
	case NO_COMMAND_LINE_VARIABLES:
		printf("No command line variables entered\n");
		break;
	case OPEN_FILE_FAILURE:
		printf("Error! Could not open the file %s\n", va_arg(p, char *));
		break;
	case LINE_TOO_LONG:
		printf("Error in Line number %d: the line is too long, the maximum line length is %d\n", va_arg(p, int), LINE_SIZE-2);
		break;
	case MEMORY_OVERFLOW:
		printf("Error in line number %d: memory allocation failed!\n", va_arg(p, int));
		exit(1);
		break;
	case UNECESSARY_COMMA:
		printf("Error in line number %d: unnecesary comma\n", va_arg(p, int));
		break;
	case TOO_MANY_COMMAS:
		printf("Error in line number %d: too many commas\n", va_arg(p, int));
		break;
	case MISSING_COMMA:
		printf("Error in line number %d: missing comma\n", va_arg(p, int));
		break;
	case SYMBOL_TOO_LONG:
		printf("Error in line number %d: symbol's name is too long. the maximum length is %d\n", va_arg(p, int), MAX_SYMBOL_LENGTH-1);
		break;
	case ILLEGAL_SYMBOL_NAME_SAVED_WORD:
		printf("Error in line number %d: the symbol's name is a saved word in assembley.\n", va_arg(p, int));
		break;
	case ILLEGAL_SYMBOL_NAME_NOT_ALPHA:
		printf("Error in line number %d: the symbol's name doesn't begin with a letter.\n", va_arg(p, int));
		break;
	case ILLEGAL_CHARACTERS_IN_SYMBOL_NAME:
		printf("Error in line number %d: the symbol's name contains illegal charecters. it must contain only numbers and letters.\n", va_arg(p, int));
		break;
	case DUPLICATE_SYMBOL:
		printf("Error in line number %d: the symbol's name already exists. multiple symbols with the same name are not allowed.\n", va_arg(p, int));
		break;
	case NOT_A_NUMBER:
		printf("Error in line number %d: after '.data' only numbers are expected. a non-numeric charecter was entered.\n", va_arg(p, int));
		break;
	case NUMBER_OUT_OF_RANGE:
		printf("Error in line number %d: a number out of range was entered.\n", va_arg(p, int));
		break;
	case ILLEGAL_STRING_FORMAT:
		printf("Error in line number %d: the string format is incorrect. strings must begin and end with '\"', for example: \"abc\"\n", va_arg(p, int));
		break;
	case UNDEFINED_COMMAND:
		printf("Error in line number %d: an undefined command was entered\n", va_arg(p, int));
		break;
	case EXCESSIVE_INPUT:
		printf("Error in line number %d: excess input found\n", va_arg(p, int));
		break;	
	case ILLEGAL_ADDRESSING_METHOD:
		printf("Error in line number %d: the operands entered are not legal for the command. Each command must get a specific number of operands with specific addressing methods.\n", va_arg(p, int));
		break;
	case ENTRY_NOT_DEFINED:
		printf("Error in line number %d: cannot use an .entry symbol without definition or if the symbol was defined externally\n", va_arg(p, int));
		break;
	case SYMBOL_NOT_DEFINED:
		printf("Error in line number %d: a symbol that wasn't declared was used as an operand\n", va_arg(p, int));
		break;	
	case CREATE_FILE_FAILED:
		printf("Error! file creation was unsuccessful.\n");
		break;
	va_end(p);
	}
	return error_flag;
}
