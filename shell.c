#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

#define BUFFER_SIZE 1024

int should_run = 1;

// Signal handler to catch Ctrl+c
void sigint_handler(int sig)
{
    printf("\nCtrl+c caught, stopping process\n");
}

// Function to execute a command and return the output
void execute_command(char *command, int output_fd)
{
    // Split the command into separate arguments
    char *args[BUFFER_SIZE];
    int argc = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL) {
        args[argc++] = arg;
        arg = strtok(NULL, " ");
    }
    args[argc] = NULL;

    // Execute the command with output redirected to the file
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (output_fd != -1) {
            // Redirect stdout to the output file
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        execvp(args[0], args);
        // If execvp returns, there was an error
        printf("Error executing command\n");
        exit(1);
    } else if (pid < 0) {
        // Fork failed
        printf("Error forking process\n");
    } else {
        // Parent process
        wait(NULL);
    }
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    while (should_run) {
        // Print shell prompt
        printf("stshell> ");
        fflush(stdout);

        // Read input from user
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove trailing newline from input
        if (buffer[strlen(buffer)-1] == '\n') {
            buffer[strlen(buffer)-1] = '\0';
        }

        // Check if user wants to exit
        if (strcmp(buffer, "exit") == 0) {
            should_run = 0;
            continue;
        }

        // Check if output should be redirected
        int output_fd = -1;
        if (strstr(buffer, ">") != NULL) {
            // Get the output filename
            char *filename = strtok(buffer, " ");
            while (filename != NULL) {
                if (strcmp(filename, ">") == 0) {
                    filename = strtok(NULL, " ");
                    break;
                }
                filename = strtok(NULL, " ");
            }
            // Open the output file for writing
            output_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }

        // Check if input should be piped
        int pipefd[2];
        int input_fd = -1;
        char *pipe_command = NULL;
        if (strstr(buffer, "|") != NULL) {
            // Get the pipe command
            pipe_command = strtok(buffer, "|");
            // Create pipe
            if (pipe(pipefd) == -1) {
                printf("Error creating pipe\n");
                continue;
            }
            // Fork process to execute the first command
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execute_command(pipe_command, -1);
                exit(0);
            } else if (pid < 0) {
                // Fork failed
                printf("Error forking process\n");
                continue;
        } else {
            // Parent process
            close(pipefd[1]);
            input_fd = pipefd[0];
        }
        // Get the next pipe command
        pipe_command = strtok(NULL, "|");
    }

    // Execute the command(s)
    while (pipe_command != NULL) {
        // Check if this is the last command in the pipe chain
        if (strtok(NULL, "|") == NULL) {
            // Use output_fd for the last command's output
            execute_command(pipe_command, output_fd);
        } else {
            // Create pipe
            if (pipe(pipefd) == -1) {
                printf("Error creating pipe\n");
                break;
            }
            // Fork process to execute the command
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                close(pipefd[0]);
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execute_command(pipe_command, -1);
                exit(0);
            } else if (pid < 0) {
                // Fork failed
                printf("Error forking process\n");
                break;
            } else {
                // Parent process
                close(pipefd[1]);
                close(input_fd);
                input_fd = pipefd[0];
            }
        }
        // Get the next pipe command
        pipe_command = strtok(NULL, "|");
    }

    // Close any open file descriptors
    if (output_fd != -1) {
        close(output_fd);
    }
    if (input_fd != -1) {
        close(input_fd);
    }
}

return 0;
}