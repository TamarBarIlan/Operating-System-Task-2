#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

void handle_signail(int sig)
{
	// we do nothing for ignore the signal
}

int main()
{
	int i;
	char *argv[10];
	char command[1024];
	char *token;

	// chek if user enter ctrl+c
	signal(SIGINT, handle_signail); // for ignore if get ctrl+c

	while (1)
	{
		printf("\033[1;32m"); // Set the text color to green and bold
		printf("stshell> ");
		printf("\033[0m"); // Reset the text color to the default

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

		// Check if user wants to exit
		if (strcmp(command, "exit") == 0)
		{
			return 0;
		}

		pid_t pid = fork();
		if (pid < 0)
		{
			exit(1);
		}
		if (pid == 0) // child
		{
			// for ctrl+c
			signal(SIGINT, SIG_DFL); // for do a difult ctrl+c

			for (int j = 0; argv[j] != NULL; j++)
			{
				if (strcmp(argv[j], ">>") == 0)
				{
					argv[j] = NULL;
					char *text = argv[j + 1];
					int outA = open(text, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
					dup2(outA, STDOUT_FILENO);
					close(outA);
					break;
				}
				if (strcmp(argv[j], ">") == 0) // out
				{
					argv[j] = NULL;
					char *text = argv[j + 1];
					// argv[j + 1] = NULL; // ****
					int out = open(text, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
					dup2(out, STDOUT_FILENO);
					close(out);
					break;
				}
				if (strcmp(argv[j], "|") == 0)
				{
					int p[2];
					if (pipe(p) < 0)
					{
						exit(1);
					}

					pid_t pid2 = fork();
					if (pid2 < 0)
					{
						exit(1);
					}

					if (pid2 == 0)
					{
						// the left command
						close(p[0]);
						int i = 0;
						char *left[10];
						while (strcmp(argv[i], "|"))
						{
							left[i] = argv[i];
							i++;
						}
						left[i] = NULL;
						int dup1 = dup2(p[1], 1);
						if (dup1 < 0)
							exit(1);
						close(p[1]);
						execvp(left[0], left);
					}
					else
					{
						// the right command
						close(p[1]);
						int i = 0;
						for (int k = j + 1; argv[k] != NULL; k++)
						{
							if (strcmp(argv[k], ">") == 0 || strcmp(argv[k], ">>") == 0)
							{
								if (strcmp(argv[k], ">") == 0) // out
								{
									// argv[k] = NULL;
									char *text = argv[k + 1];
									int out = open(text, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
									dup2(out, STDOUT_FILENO);
									close(out);
									break;
								}
								argv[i] = NULL;
							}
							else
							{
								argv[i] = argv[k];
							}

							i++;
						}
						argv[i] = NULL;

						int dup = dup2(p[0], 0);
						if (dup < 0)
							exit(1);
						close(p[0]);
						wait(NULL);
					}
				}

			} // for
			execvp(argv[0], argv);
		} // if
		wait(NULL);
	} // while
}