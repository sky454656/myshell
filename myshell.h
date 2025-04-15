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
#include <limits.h>

int run_command(char * cmd, int sep);
int run_pipe(char (*command)[100], int count);
void my_strtok(char *cmd, char **argv);

void split_line(char *line, int *separator, char command[][100]);
void split_separator(char *line, int*separator);
char *strtrim(char * str);
int isseparator(char *s);

void print_interface();

int myshell_pwd(char *path, size_t size);
int myshell_cd(char * path);



#endif