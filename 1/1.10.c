/*
	modshell.c - программа для частичной иммитации командной оболочки.
	
	Данная программа является улучшением modshell.c, позволяющая использовать конвейеры 
	команд произвольной длинны.
**/

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

void get_all_args(char *all_usr_args[], char *usr_input) {
	
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
			strcpy(all_usr_args[i], token);
		else 
			all_usr_args[i] = NULL;
		i++;
	} while (token != NULL);
}

void get_curr_args(char *all_usr_args[], char *curr_usr_args[], int a_serial_num) {

	int i, j, cmd_num;
	
	cmd_num = 1; j = 0;
	for (i = 0; (all_usr_args[i] != NULL) && (cmd_num <= a_serial_num); i++) {
		if (all_usr_args[i][0] == '|') {
			cmd_num++;
			i++;
		}
		if (cmd_num == a_serial_num) {
			strcpy(curr_usr_args[j], all_usr_args[i]);
			j++;
		}
	}
	curr_usr_args[j] = NULL;
}

int get_count_cmd(char *usr_args[]) {
	
	int i, count;
	
	count = 1; 
	for (i = 0; usr_args[i] != NULL; i++)
		if ((usr_args[i][0] == '|') && (usr_args[i + 1] != NULL))
			count++;

	return count;
}

void start_cmd(char *usr_args[], char *envp[]) {

    pid_t pid;

	execve(usr_args[0], usr_args, envp);
	execvpe(usr_args[0], usr_args, envp);
	if (usr_args[0] != NULL)
		printf("%s: command not found\n", usr_args[0]);
}

int main(int argc, char *argv[], char *envp[]) {
    
    pid_t pid;
	char *all_args[MAX_NUMBER_ARG], *curr_args[MAX_NUMBER_ARG];
	char usrin[MAX_INPUT_SIZE];
	int i, j;
	int count_cmd, serial_num;
	int fd[2], f_in;
	
	for (i = 0; i < MAX_NUMBER_ARG; i++)
		all_args[i] = (char *)malloc(sizeof(char) * MAX_INPUT_SIZE);
	
	do {
		//read user command
        printf("$ ");
		fgets(usrin, sizeof(usrin), stdin);
		if (usrin[0] == '\n') continue;
		usrin[strlen(usrin) - 1] = '\0';
		if (strcmp(usrin, "exit") == 0) break;
		
		//get all user commands
		get_all_args(all_args, usrin);
		count_cmd = get_count_cmd(all_args);

		if (pipe(fd) == -1) {
			printf("Error: cannot create pipe\n");
			exit(-1);
		}
		
		//cycle for pipeline
		serial_num = 0;
		while (count_cmd != serial_num) {
			
			for (i = 0; i < MAX_NUMBER_ARG; i++)
				if (serial_num == 0)
					curr_args[i] = (char *)malloc(sizeof(char) * MAX_INPUT_SIZE);
				else 
					curr_args[i] = (char *)realloc(curr_args[i], sizeof(char) * MAX_INPUT_SIZE);

			serial_num++;

			//get arguments for current command in the pipeline
			get_curr_args(all_args, curr_args, serial_num);
			
			//create proc for next command
			pid = fork();
			if (pid == -1) {
				printf("Error: cannot create new process\n");
				exit(-1);
			}
			if (pid == 0) {
				dup2(f_in, 0);
				if (count_cmd != serial_num)
					dup2(fd[1], 1);
				close(fd[1]);
				start_cmd(curr_args, envp);
				exit(EXIT_SUCCESS);
			}
			else {
				wait(NULL);
				close(fd[1]);
				f_in = fd[0];
			}

			for (i = 0; i < MAX_NUMBER_ARG; i++)
				curr_args[i] = (char *)realloc(curr_args[i], sizeof(char) * MAX_INPUT_SIZE);
		}
		
		for (i = 0; i < MAX_NUMBER_ARG; i++) 
			all_args[i] = (char *)realloc(all_args[i], sizeof(char) * MAX_INPUT_SIZE);

    } while (1);
	
	for (i = 0; i < MAX_NUMBER_ARG; i++)
		free(all_args[i]);

    return 0;
}
