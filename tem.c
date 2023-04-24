#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_COMMAND_LENGTH 100

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int status;

    while(1) {
        printf("stshell> ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Remove the newline character from the end of the command string
        command[strcspn(command, "\n")] = '\0';

        // Tokenize the command string into arguments
        char *token = strtok(command, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        // Exit the shell if the user enters "exit"
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                printf("stshell: %s: command not found\n", args[0]);
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            // Forking error
            printf("stshell: forking error\n");
        } else {
            // Parent process
            wait(&status);
        }
    }

    return 0;
}