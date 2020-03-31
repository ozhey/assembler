assembler: assembler.o print_errors.o auxiliary_func.o first_iteration.o symbol_list.o output.o second_iteration.o data_list.o assembler.h
	gcc -g -Wall -ansi -pedantic print_errors.o assembler.o auxiliary_func.o first_iteration.o second_iteration.o output.o symbol_list.o data_list.o -o assembler
assembler.o: assembler.c assembler.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
auxiliary_func.o: auxiliary_func.c assembler.h
	gcc -c -Wall -ansi -pedantic auxiliary_func.c -o auxiliary_func.o
print_errors.o: print_errors.c assembler.h
	gcc -c -Wall -ansi -pedantic print_errors.c -o print_errors.o
first_iteration.o: first_iteration.c assembler.h
	gcc -c -Wall -ansi -pedantic first_iteration.c -o first_iteration.o
second_iteration.o: second_iteration.c assembler.h
	gcc -c -Wall -ansi -pedantic second_iteration.c -o second_iteration.o
output.o: output.c assembler.h
	gcc -c -Wall -ansi -pedantic output.c -o output.o
symbol_list.o: symbol_list.c assembler.h
	gcc -c -Wall -ansi -pedantic symbol_list.c -o symbol_list.o
data_list.o: data_list.c assembler.h
	gcc -c -Wall -ansi -pedantic data_list.c -o data_list.o
