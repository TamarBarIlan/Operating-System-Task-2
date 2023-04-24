#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

int main()
{
    int i;
    char *argv[10];
    char command[1024];
    char *token;

    while (1)
    {
        printf("myshell> "); // add prompt
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0

        /* parse command line by " "*/
        i = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL;

        /* Is command empty */
        if (argv[0] == NULL)
            continue;

        pid_t pid = fork();
        if(pid<0){
            perror("fork error");
            exit(1);
        }
        if (pid == 0) // child
        {
            printf("Child PID: %d, Executing Command: %s\n", getpid(), argv[0]); // add print statement
            for (int j = 0; argv[j] != NULL; j++)
            {
                if (strcmp(argv[j], "|") == 0)
                {
                    printf("in  | \n");
                    int p[2];
                    if(pipe(p)<0){
                        perror("pipe error");
                        exit(1);
                    }

                    pid_t pid2 = fork();
                    if(pid2<0){
                        perror("fork error");
                        exit(1);
                    }
                    printf("after fork \n");


                    if(pid2==0){
                        printf("left command.. \n");

                        //the left command
                        close(p[0]);
                        int i=0;
                        char *left[10];
                        while(strcmp(argv[i], "|")){
                            left[i]=argv[i];
                            i++;
                        }
                        left[i]=NULL;
                            printf("Executing Command left: %s\n" ,left[0]); // add print statement
                        if(dup2(p[1],1)<0){
                            perror("dup2 error");
                            exit(1);
                        }
                        printf("after dup \n");
                        close(p[1]);
                        execvp(left[0], left);
                        perror("execvp error"); // add error message for execvp failure
                        exit(1);
                    }else{
                        //the right command
                        printf("right.. \n");

                        int i=0;
                        for (int k = j+1; argv[k] != NULL; k++){
                            argv[i]=argv[k];
                            i++;
                        }
                        j=-1;
                        argv[i]=NULL;
                        if(dup2(p[0],0)<0){
                            perror("dup2 error");
                            exit(1);
                        }
                        close(p[0]);
                        wait(NULL);
                    }

                }

            }//for
            execvp(argv[0],argv);
            perror("execvp error"); // add error message for execvp failure
            exit(1); // exit child process after execvp
        }//if
        else {
            wait(NULL); // wait for child process only after non-pipe command
        }
    }//while
}
