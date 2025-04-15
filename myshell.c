#include "myshell.h"

int main()
{
    while (1)
    {
        char *line = NULL;
        size_t len = 0;
        print_interface();
        getline(&line, &len, stdin);

        
        if (strcmp(line, "exit\n") == 0)
            exit(0);

        char command[100][100];

        memset(command, 0, sizeof(command));
        int separator[100];
        split_line(line, separator, command); // | = 1 ; = 2 && = 3 || = 4 & = 5 xx : 0

        free(line);

        int tmp_separator;
        int i = 0;
        while(command[i][0] != '\0')
        {
            int ret;

            tmp_separator = separator[i];
            if (separator[i] == 1)
            {
                int num_of_pipe = 0;
                while (separator[i + num_of_pipe] == 1 && separator[i + num_of_pipe])
                    num_of_pipe++;

                ret  = run_pipe(&command[i], num_of_pipe + 1);
                i += num_of_pipe;
                tmp_separator = separator[i];
            }
            else
                ret = run_command(command[i], separator[i]);
            i++;
    
            if (tmp_separator == 3 && ret) // &&
            {
                i++;
                break;
            }
            if (tmp_separator == 4 && !ret) // ||
            {
                i++;
                break;
            }
        }

    }
}

// 가장 작은 프로세스 작동 단위 , 파이프는 따로 처리
int run_command(char * cmd, int sep)
{
    //명령어 파싱
    char *argv[100];
    int i = 0;
    char tmp_command[100];

    strcpy(tmp_command, cmd);
    char *token = strtok(tmp_command, " ");
    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    //cd 처리 
    if (!strcmp(argv[0], "cd"))
    {
        char * dir;
        char path[PATH_MAX];
    
        if (argv[1] == NULL)
            dir = getenv("HOME");
        else
            dir = argv[1];
          
        int cd_ret = myshell_cd(dir);
        return (cd_ret);
    }
    pid_t pid = fork();

    if (pid == 0) //child process
    {
        if (!strcmp(argv[0], "pwd"))
        {
            char path[PATH_MAX];
            int cwd_ret = myshell_pwd(path, sizeof(path));
            printf("%s\n", path);
            exit(cwd_ret);
        }
        else
        {
            execvp(argv[0], argv);
            perror("exec fail 1"); // ||, && 판단 기준
            //execvp가 정상적으로 실행되었을 경우 다음 줄 실행 x 
            exit(1);
        }
    }
    else // parent process
    {
        int status;
        if (sep != 5)
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                return 0; //성공시 0, 실패시 1;
            return 1;
        }
        else // 백그라운드 실행은 성공으로 간주하고 다음 명령어 실행
            return 0; 
    }
}


int run_pipe(char (*command)[100], int count)
{
    int ret;
    int stdin_copy = dup(STDIN_FILENO);

    //명령어 파싱
    char *argv[100];
    int i = 0;
    char tmp_command[100];

    strcpy(tmp_command, command[0]);
    char *token = strtok(tmp_command, " ");
    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;


    if (count == 1)
    {
        pid_t pid = fork();
        
        if (pid == 0)
        {
            if (!strcmp(command[0], "pwd"))
            {
                char path[PATH_MAX];
                int cwd_ret = myshell_pwd(path, sizeof(path));
                printf("my_pwd : %s\n", path);
                dup2(stdin_copy, STDIN_FILENO);
                close(stdin_copy);
                exit(cwd_ret);
            }
            execvp(argv[0], argv);
            perror("exec fail 2");
            exit(1);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            dup2(stdin_copy, STDIN_FILENO);
            close(stdin_copy);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                return 0; //성공시 0, 실패시 1;
            return 1;
        }
    }

    int pipefd[2];
    pipe(pipefd);

    
    pid_t pid = fork();

    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execvp(argv[0], argv);
        perror("exec fail 3");
        exit(1);
    } 
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            return 1; // 실패시 1;

        close(pipefd[1]); 
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        ret = run_pipe(command + 1, count - 1);
        
        dup2(stdin_copy, STDIN_FILENO);
        close(stdin_copy);
    }
    return ret;
}
