#include "myshell.h"

int myshell_pwd(char *path, size_t size)
{
    if (getcwd(path, size) == NULL)
        return 1;
    else
        return 0; // 성공시 0 실패시 1
}

int myshell_cd(char * path)
{
    if (chdir(path) == 0)
        return 0;
    else
        return 1; // 성공시 0 실패시 1
}


