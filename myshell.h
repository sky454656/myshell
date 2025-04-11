#ifndef MYSHELL_H
# define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>

int run_command(char * cmd, int sep);
int run_pipe(char (*command)[100], int count);


void split_line(char *line, int *separator, char command[][100]);
void split_separator(char *line, int*separator);
char *strtrim(char * str);
int isseparator(char *s);

#endif