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

        char command[256][256];

        memset(command, 0, sizeof(command));
        int separator[256];
        split_line(line, separator, command); // | = 1 ; = 2 && = 3 || = 4 & = 5 xx : 0

        free(line);

        int tmp_separator;
        int i = 0;
        while(command[i][0] != '\0')
        {
            int ret;
            tmp_separator = separator[i];

            int flag = 0;

            if (separator[i] == 1) //파이프에서의 cd 처리
            {
                int num_of_pipe = 0;
                while (separator[i + num_of_pipe] == 1)
                    num_of_pipe++; 

                for (int j = i + num_of_pipe ; j >= i; j--)
                {
                    if (!strcmp(command[j], "cd"))
                    {
                        if (separator[j] == 4)
                            flag = 2;
                        else
                        {
                            i = j + 1;
                            flag = 1;
                        }
                        
                        break;
                    }           
                }    

            }
            if (flag == 1)
                continue;
            else if (flag == 2)
                break;

            if (separator[i] == 1) //파이프 처리
            {
                int num_of_pipe = 0;
                while (separator[i + num_of_pipe] == 1)
                    num_of_pipe++;

                ret  = run_pipe(&command[i], num_of_pipe + 1);
                i += num_of_pipe;
                tmp_separator = separator[i];
            }
            else
                ret = run_command(command[i], separator[i]);
            i++;
    
            if (tmp_separator == 3 && ret) // &&
                break;
            if (tmp_separator == 4 && !ret) // ||
                break;
        }

    }
}

// 가장 작은 프로세스 작동 단위 , 파이프는 따로 처리
int run_command(char * cmd, int sep)
{
    //명령어 파싱
    char *argv[256];
    int i = 0;
    char tmp_command[256];

    strcpy(tmp_command, cmd);
    char *token = strtok(tmp_command, " ");
    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    //cd 예외 처리 
    // 1) cda 같이 잘못들어온 명령어
    // 2) 인자 2개이상
    if (!strncmp(argv[0], "cd", 2) && strcmp(argv[0], "cd") != 0)
    {
        printf("command '%s' not found\n", argv[0]);
        return 1;
    }
    if (!strcmp(argv[0], "cd") && argv[2] != NULL)
    {
        printf("-bash: cd: too many argumets\n");
        return 1;
    }

    //cd 처리
    //없는 디렉토리로 이동시 처리
    if (!strcmp(argv[0], "cd"))
    {
        char * dir;
        char path[PATH_MAX];
    
        if (argv[1] == NULL)
            dir = getenv("HOME");
        else
            dir = argv[1];
          
        int cd_ret = myshell_cd(dir);
        if(cd_ret)
            printf("-bash: cd: %s: No such file or directory\n", argv[1]);
        return (cd_ret);
    }

    pid_t pid = fork();

    if (pid == 0) //child process
    {
        if (!strcmp(argv[0], "pwd")) //pwd 처리
        {
            char path[PATH_MAX];
            int cwd_ret = myshell_pwd(path, sizeof(path));
            printf("%s\n", path);
            exit(cwd_ret);
        }
        else
        {
            execvp(argv[0], argv);
            fprintf(stderr, "%s: command not found\n", argv[0]); // ||, && 판단 기준
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


int run_pipe(char (*command)[256], int count)
{
    int ret;
    int stdin_copy = dup(STDIN_FILENO);

    //명령어 파싱
    char *argv[256];
    int i = 0;
    char tmp_command[256];

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
                printf("%s\n", path);
                dup2(stdin_copy, STDIN_FILENO);
                close(stdin_copy);
                exit(cwd_ret);
            }
            execvp(argv[0], argv);
            perror("exec fail 1");
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
        perror("exec fail 2");
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
