#include "assembler.h"

/*this function removes white charecters and commas from the begginning of a string
 *the functions also returns the number of commas found*/
int remove_white(char *line)
{
	char *ptr = line;
	int comma_count = 0;
	while (*ptr == ' ' || *ptr == '\t' || *ptr == ',') 
	{
		if (*ptr == ',')
			comma_count++;
		ptr++;
	}
	strcpy(line,ptr);
	return comma_count;
}

/*this function gets an integer that represents the number of commas found and prints an error if it's not 1
 * the function returns true if the number is 1 or false otherwise */
int one_comma(int comma, int lc)
{
	if (comma != 1) 
	{
		if (comma > 1)
			print_errors(TOO_MANY_COMMAS,lc);
		else if (comma == 0)
			print_errors(MISSING_COMMA,lc);
		return FALSE;
	}
	return TRUE;		
}

/*This function trims the next word from the line and place it into the word variable.
The function also rids of white charecters after the first word*/
void get_word(char *word, char *line)
{
	char *ptr = line; /*A pointer to the input string*/
	while ((*ptr != ' ') && (*ptr != '\t') && (*ptr != ',') && (*ptr != '\0') && (*ptr != '\n'))
		ptr++;
	strncpy(word,line,ptr-line); /*Copy the next word to the token*/
	word[ptr-line] = '\0';
	strcpy(line, ptr);
}

/*this function get a string array, a word, the array begin index and the array end index and check whether the word is in the array between the indexs (return TRUE (1)) or not in the array between the indexs (return FALSE (0))*/
int is_appears(char ** array, char * word, int beg_index, int end_index)
{
	char ** ptr = array;
	int i;
	for (i = beg_index; i < end_index; i++)
	{
		if (!(strcmp(ptr[i], word)))/*true - word apears in array*/
			return TRUE;
	}
	return FALSE;
}
