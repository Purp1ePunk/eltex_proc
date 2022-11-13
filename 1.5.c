// 1.5 (3 балла). Усовершенствовать программу 1.4: добавить
// возможность запуска программ, установленных в системе.

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_NUMBER_ARG (MAX_INPUT_SIZE / 2)

void get_usr_arg(char *usr_arg[], char *usr_input) {

	char *token;
	int i;

	//token = command or option or attribute or, etc
	i = 0;
	do {
		if (i == 0)
			token = strtok(usr_input, " \t");
		else
			token = strtok(NULL, " \t");
		if (token != NULL)
			strcpy(usr_arg[i], token);
		else
			usr_arg[i] = NULL;
		i++;
	} while (token != NULL);
}

int main(int argc, char *argv[], char *envp[]) {

    pid_t pid;
	char *args[MAX_NUMBER_ARG];
	char usrin[MAX_INPUT_SIZE];
	int i, j;

	//allocate mem
	for (i = 0; i < MAX_NUMBER_ARG; i++)
		args[i] = (char *)malloc(sizeof(char) * MAX_INPUT_SIZE);

	do {
		//read user command
        printf("$ ");
		fgets(usrin, sizeof(usrin), stdin);
		if (usrin[0] == '\n') continue;
		usrin[strlen(usrin) - 1] = '\0';
		if (strcmp(usrin, "exit") == 0) break;

		//get user command
		get_usr_arg(args, usrin);

		//create new proc for exec()
        pid = fork();
        if (pid == -1) {
            printf("Error: cannot create new process\n");
            exit(-1);
        }
        else if (pid == 0) {
            execve(args[0], args, envp);
            execvpe(args[0], args, envp);
			if (args[0] != NULL)
				printf("%s: command not found\n", args[0]);
			exit(EXIT_SUCCESS);
		}
		else
			wait(NULL);

		//reallocate mem
		for (i = 0; i < MAX_NUMBER_ARG; i++)
			args[i] = (char *)realloc(args[i], sizeof(char) * MAX_INPUT_SIZE);
    } while (1);

	for (i = 0; i < MAX_NUMBER_ARG; i++)
		free(args[i]);

    return 0;
}
