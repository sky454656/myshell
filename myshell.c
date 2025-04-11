#include "myshell.h"

int main()
{
    char *line = NULL;
    size_t len = 0;

   // while (1)
  //  {
        tcflush(STDIN_FILENO, TCIFLUSH);
        printf("interface");
        getline(&line, &len, stdin);
        //인터페이스 출력 kseo@user:pwd$

        if (strcmp(line, "exit\n") == 0)
            return 0;


        char command[100][100];

        memset(command, 0, sizeof(command));
        int separator[100];
        split_line(line, separator, command); // | = 1 ; = 2 && = 3 || = 4 & = 5 xx : 0

        int i = 0;
        while(command[i][0] != '\0')
        {
            int ret;

            if (separator[i] == 1)
            {
                int num_of_pipe = 0;
                while (separator[i + num_of_pipe] == 1 && separator[i + num_of_pipe])
                    num_of_pipe++;

                ret  = run_pipe(&command[i], num_of_pipe + 1);
                i += num_of_pipe;
            }
            else
            {
                ret = run_command(command[i], separator[i]);
                i++;
            }
        
            if (separator[i] == 3 && ret) // &&
                break;
            if (separator[i] == 4 && !ret) // ||        
                break;
        }

 //   }

    
}

// 가장 작은 프로세스 작동 단위 , 파이프는 따로 처리
int run_command(char * cmd, int sep)
{
    pid_t pid = fork();

    char *argv[100];
    int i = 0;

    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    if (pid == 0) //child process
    {
        execvp(argv[0], argv);
        perror("exec fail"); // ||, && 판단 기준
        //execvp가 정상적으로 실행되었을 경우 다음 줄 실행 x 
        exit(1);
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

    char *argv[100];
    int i = 0;

    char *token = strtok(command[0], " ");
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
            execvp(argv[0], argv);
            perror("execvp fail");
            exit(1);
        }
        else 
        {
            int status;
            waitpid(pid, &status, 0);
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
        perror("execvp fail");
        exit(1);
    } 
    else
    {
        waitpid(pid, NULL, 0);

        close(pipefd[1]); 
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        ret = run_pipe(command + 1, count - 1);
    }
    return ret;
}
