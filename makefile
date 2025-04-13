
myshell : myshell.o myshell_interface.o split_line.o myshell_utils.o
	gcc -o myshell myshell.o myshell_interface.o split_line.o myshell_utils.o

myshell.o : myshell.h myshell.c
	gcc -c -o myshell.o myshell.c

myshell_interface.o : myshell.h myshell_interface.c
	gcc -c -o myshell_interface.o myshell_interface.c

split_line.o : myshell.h split_line.c
	gcc -c -o split_line.o split_line.c

myshell_utils.o : myshell.h myshell_utils.c
	gcc -c -o myshell_utils.o myshell_utils.c

clean:
	rm -f myshell *.o