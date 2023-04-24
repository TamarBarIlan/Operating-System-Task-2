#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length of a command */
#define MAX_ARGS 10 /* The maximum number of arguments in a command */

int main(void) {
    char *args[MAX_ARGS]; /* Array of pointers to arguments */
    char line[MAX_LINE]; /* The user's command line input */
    pid_t pid; /* Process ID for child process */
    int status; /* Exit status of child process */
    int p[2]; /* File descriptors for pipe */
    int should_pipe = 0; /* Flag indicating whether to pipe commands */

    while (1) {
        printf("shell> ");
        fflush(stdout);
        fgets(line, MAX_LINE, stdin); /* Read user's input */
        line[strlen(line) - 1] = '\0'; /* Remove newline character */

        /* Split user's input into individual arguments */
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; /* Add null pointer to end of argument array */

        /* Check if user wants to pipe commands */
        for (i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "|") == 0) {
                should_pipe = 1;
                args[i] = NULL; /* Replace "|" with null pointer */
                /* Set up pipe */
                if (pipe(p) < 0) {
                    perror("pipe");
                    exit(1);
                }
                break;
            }
        }

        /* Fork a child process to execute the command */
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { /* Child process */
            if (should_pipe) { /* First command in pipeline */
                close(STDOUT_FILENO); /* Close standard output */
                dup(p[1]); /* Redirect standard output to pipe */
                close(p[0]);
                close(p[1]);
            }
            execvp(args[0], args); /* Execute the command */
            perror("execvp"); /* If execvp fails, print error message */
            exit(1);
        } else { /* Parent process */
            if (should_pipe) { /* Second command in pipeline */
                close(STDIN_FILENO); /* Close standard input */
                dup(p[0]); /* Redirect standard input to pipe */
                close(p[0]);
                close(p[1]);
                /* Execute the second command in the pipeline */
                pid = fork();
                if (pid < 0) {
                    perror("fork");
                    exit(1);
                } else if (pid == 0) {
                    execvp(args[i+1], &args[i+1]);
                    perror("execvp");
                    exit(1);
                } else {
                    waitpid(pid, &status, 0);
                }
            }
            waitpid(pid, &status, 0); /* Wait for child process to finish */
        }
        should_pipe = 0; /* Reset pipe flag */
    }
    return 0;
}