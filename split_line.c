#include "myshell.h"

void split_line(char *line, int *separator, char command[][100])
{
    char tmp[100];
    int j;
    int k = 0;
    split_separator(line, separator);
   
    
    for (int i = 0; line[i]; i += j)
    {
        memset(tmp, 0, sizeof(tmp));
        j = 0;
        while (!(isseparator(line + i + j)) && line[i + j] != '\0')
            j++;
        strncpy(tmp, line + i, j);
        tmp[j] = '\0';
        strcpy(command[k], strtrim(tmp));
        k++;
        j += isseparator(line + i + j);
    }
}

void split_separator(char *line, int *separator)
{
    // | = 1 ; = 2 && = 3 || = 4 & = 5 xx : 0
    int i = 0;
    int j = 0;
    while (line[i])
    {
        if (line[i] == ';'){
            separator[j] = 2;
            j++;
        }
        else if (line[i] == '|' && line[i+1] != '|'){
            separator[j] = 1;
            j++;
        }
        else if (line[i] == '&' && line[i+1] != '&'){
            separator[j] = 5;
            j++;
        }
        else if (line[i] == '|'){
            separator[j] = 4;
            j++;
            i++;
        }
        else if (line[i] == '&'){
            separator[j] = 3;
            j++;
            i++;
        }
        i++;
    }
    separator[j] = 0;
}

int isseparator(char *s) //~separator-> 0
{
    if (*s == ';' )
        return 1;
    else if (*s == '|' && *(s+1) != '|')
        return 1;
    else if (*s == '&' && *(s+1) != '&')
        return 1;
    else if (*s == '|' || *s == '&')
        return 2;
    else
        return 0;
}

char *strtrim(char * str)
{
    while (isspace(*str))
        str++;

    char * end = str + strlen(str) -  1;
    while (end > str && isspace(*end))
        end--;
    *(end + 1) = '\0';

    return str;
}
    