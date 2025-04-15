#include "myshell.h"


void print_interface()
{
    char *user = getenv("USER");
    char hostname[100];

    if (gethostname(hostname, 100) == 1)
        exit(1);

    char path[PATH_MAX];
    int cwd_ret = myshell_pwd(path, sizeof(path));

    if (strcmp(path, getenv("HOME")) == 0)
        strcpy(path, "~");


    printf("%s@%s:%s$ ", user, hostname, path);
}