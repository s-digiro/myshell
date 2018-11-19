/* Filename:	main.c
 * Author:	Sean DiGirolamo
 * Version:	1.0.0
 * Date:	03-07-18
 * Purpose:	Main method and functions for myshell program. Does not have a
 *		header, doesn't need one.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "command.h"
#include "filedir.h"
#include "built_ins.h"



/* Prints command prompt to terminal */
void print_prompt();

/* Reads a line from the terminal */
void read_line(char ** retval);

int find_last(char * s, char c);
void set_shell_dir_env(char ** shell_path);
void set_shell_env(char ** shell_path);
void set_parent_env();
void sig_handler(int sig);
int is_all_whitespace(char * s);



int ampersand_flag;
int arg_flag;
int builtin_flag;



int main(int argc, char **argv)
{
	ampersand_flag = 0;
	arg_flag = 0;

	// Check for batchfile
	if (argc > 1 && file_exists(argv[1])) {
		arg_flag = 1;

		freopen(argv[1], "r", stdin);
	} else if (argc > 1) {
		printf("File not found\n");
		exit(0);
	}

	signal(SIGCHLD, sig_handler);

	// Set shell environment variable
	char * shell_path = NULL;
	char * shelldir_path = NULL;
	set_shell_env(&shell_path);
	set_shell_dir_env(&shelldir_path);

	// Loops infinitely
	while (1) {
START:
		if (arg_flag == 0) {
			print_prompt();
		}

		ampersand_flag = 0;

		/* Get line of command from input */
		char * line = NULL;
		read_line(&line);


		// Catches case where user does not input anything, or where
		// the input is invalid for whatever reason
		if (is_all_whitespace(line)) {
			// Case: User hasn't actually entered anything
			free(line);
			continue;
		} else if (line[0] == '|' || line[0] == '<'
		           || line[0] == '>') {
			// Case: Invalid input
			printf("Invalid input:\n");
			printf("Command cannot start with </|/>\n");

			free(line);
			continue;
		}


		/* Parse command into struct Command */
		COMMAND * com = malloc(sizeof(COMMAND) * 1);
		if (commandify(com, line, '|') == -1) {
			printf("myshell: Parse error.\n");

			free(line);
			free_com(com);
			continue;
		}


		/* Check for quit, and cd, which have unique properties
		 * and must be dealth with in main program, not a child
		 */
		if (strcmp(com->segs[0]->seg[0], "quit") == 0) {
			free(line);
			free_com(com);
			break;
		} else if (strcmp(com->segs[0]->seg[0], "cd") == 0) {
			cd(com->segs[0]);
		}
		COM_SEG * last_seg = com->segs[com->segc - 1];
		int * last_seg_strc = &(last_seg->strc);
		char ** last_string = &(last_seg->seg[*last_seg_strc - 1]);
		if (strcmp(*last_string, "&") == 0) {
			ampersand_flag = 1;

			free(*last_string);
			*last_string = NULL;
			last_seg->strc--;
		}

		// Make sure all commands exist
		for (int i = 0; i < com->segc; i++) {
			char * filepath = get_path(com->segs[i]->seg[0]);
			int x = !file_exists(filepath);
			free(filepath);
			if (x && !is_built_in(com->segs[i]->seg[0])) {
				printf("Error: Command %s not found.\n", com->segs[i]->seg[0]);

				free(line);
				free_com(com);
				goto START;
			}
		}

		/* This is the ugly part for executing the command and
		 * handling input redirection
		 */
		int pipec = com->segc - 1;
		int **fd;
		pid_t pid;
		int status;
		int x;
		/* If we have pipes... */
		if (pipec > 0) {
			// Case: We have pipes

			// Malloc data for pipes
			fd = malloc(sizeof(int *) * pipec);
			for (int i = 0; i < pipec; i++) {
				fd[i] = malloc(sizeof(int) * 2);
				pipe(fd[i]);
			}

			// Iterate through each pipe separated segment of
			// command
			for (int i = 0; i < com->segc; i++) {
				// Get current segment and current string count
				// for readability
				COM_SEG * cur_segment = com->segs[i];
				int * cur_strc = &(cur_segment->strc);

				// Fork Parent (Will fork once for each process)
				pid = fork();
				if (pid == 0) {
					// Child

					// Set parent env variable
					set_parent_env();

					// Handle Input redirection right here
					int index;
					char * filename;
					if ((index = search_seg(cur_segment, "<")) >= 0) {
						// Contains <

						// Rerout stdin to file after <
						filename = cur_segment->seg[index + 1];
						freopen(filename, "r", stdin);

						// Modify the segment (remove < and after)
						for (int j = index; j < *cur_strc; j++) {
							free(cur_segment->seg[j]);
							cur_segment->seg[j] = NULL;
						}
						*cur_strc = *cur_strc - (*cur_strc - index);
					}
					if ((index = search_seg(cur_segment, ">")) >= 0) {
						// Contains >

						// Rerout stdout to file after >
						filename = cur_segment->seg[index + 1];
						remove(filename);
						freopen(filename, "a+", stdout);

						// Modify the segment (remove > and after)
						for (int j = index; j < *cur_strc; j++) {
							free(cur_segment->seg[j]);
							cur_segment->seg[j] = NULL;
						}
						*cur_strc = *cur_strc - (*cur_strc - index);
					} else if ((index = search_seg(cur_segment, ">>")) >= 0) {
						// Contains >

						// Rerout stdout to file after >
						filename = cur_segment->seg[index + 1];
						freopen(filename, "a+", stdout);

						// Modify the segment (remove > and after)
						for (int j = index; j < *cur_strc; j++) {
							free(cur_segment->seg[j]);
							cur_segment->seg[j] = NULL;
						}
						*cur_strc = *cur_strc - (*cur_strc - index);
					}

					// Handle pipes right here
					// Close unused parts and redirect
					// other parts to stdin/stdout
					for (int j = 0; j < pipec; j++) {
						if (j == i) {
							// stdout pipe
							close(fd[j][0]);
							dup2(fd[j][1], 1);
						} else if (j == i - 1) {
							// stdin pipe
							close(fd[j][1]);
							dup2(fd[j][0], 0);
						} else {
							// Close both ends
							close(fd[j][1]);
							close(fd[j][0]);
						}
					}

					// Cuts off pipe from string (we don't
					// want to execute the pipe or
					// afterwards. That would be weird)
					if (i < com->segc - 1) {
						com->segs[i]->seg[com->segs[i]->strc - 1] = '\0';
					}

					x = i;
					break;
				} else if (pid < 0) {
					// Fork Error
					printf("Fork error\n");
				}
			}
			if (pid > 0) {
				// Parent
				for (int i = 0; i < pipec; i++) {
					close(fd[i][0]);
					close(fd[i][1]);
				}
				if (ampersand_flag == 0) {
					for (int i = com->segc; i > 0; i--) {
						wait(&status);
					}
					for (int i = 0; i < pipec; i++) {
						free(fd[i]);
					}
					free(fd);
				}
			} else {
				// Child
				built_ins(com->segs[0]);
				execvp(get_path(com->segs[x]->seg[0]), com->segs[x]->seg);
			}
		} else {
			// Case: No pipes
			pid = fork();
			if (pid == 0) {
				// Child

				set_parent_env();

				COM_SEG * cur_segment = com->segs[0];
				int * cur_strc = &(cur_segment->strc);

				// Handle Input redirection right here
				int index;
				char * filename;
				if ((index = search_seg(cur_segment, "<")) >= 0) {
					// Contains <

					// Rerout stdin to file after <
					filename = cur_segment->seg[index + 1];
					freopen(filename, "r", stdin);

					// Modify the segment (remove < and after)
					for (int j = index; j < *cur_strc; j++) {
						free(cur_segment->seg[j]);
						cur_segment->seg[j] = NULL;
					}
					*cur_strc = *cur_strc - (*cur_strc - index);
				}
				if ((index = search_seg(cur_segment, ">")) >= 0) {
					// Contains >

					// Rerout stdout to file after >
					filename = cur_segment->seg[index + 1];
					remove(filename);
					freopen(filename, "a+", stdout);

					// Modify the segment (remove > and after)
					for (int j = index; j < *cur_strc; j++) {
						free(cur_segment->seg[j]);
						cur_segment->seg[j] = NULL;
					}
					*cur_strc = *cur_strc - (*cur_strc - index);
				} else if ((index = search_seg(cur_segment, ">>")) >= 0) {
					// Contains >

					// Rerout stdout to file after >
					filename = cur_segment->seg[index + 1];
					freopen(filename, "a+", stdout);

					// Modify the segment (remove > and after)
					for (int j = index; j < *cur_strc; j++) {
						free(cur_segment->seg[j]);
						cur_segment->seg[j] = NULL;
					}
					*cur_strc = *cur_strc - (*cur_strc - index);
				}

				built_ins(com->segs[0]);
				execvp(get_path(com->segs[0]->seg[0]), com->segs[0]->seg);
			} else if (pid > 0) {
				// Parent
				if (ampersand_flag == 0) {
					wait(&status);
				}
			} else {
				// Fork Error
				printf("Fork error\n");
			}
		}

		/* Free any malloced data */
		free(line);
		free_com(com);
	}
	free(shell_path);
	free(shelldir_path);

	return 0;
}

void print_prompt()
{
	char * pwd;
	int size_pwd = strlen(getenv("PWD"));
	pwd = malloc(sizeof(char) * (size_pwd + 1));
	strcpy(pwd, getenv("PWD"));
	pwd[size_pwd] = '\0';

	printf("%s> ", pwd);

	free(pwd);
}

void read_line(char ** retval)
{
	size_t n = 0;
	int num = getline(retval, &n, stdin);
	if (num == -1) {
		exit(0);
	}
}

void set_shell_dir_env(char ** shell_path)
{
	char cwd[2048];
	getcwd(cwd, 2048);

	int cwd_size = strlen(cwd);
	*shell_path = malloc(sizeof(char) * (9 + cwd_size + 1));
	strcpy(*shell_path, "shelldir=");
	strcat(*shell_path, cwd);
	(*shell_path)[9 + cwd_size] = '\0';

	putenv(*shell_path);
}

void set_shell_env(char ** shell_path)
{
	char cwd[2048];
	getcwd(cwd, 2048);

	int cwd_size = strlen(cwd);
	*shell_path = malloc(sizeof(char) * (6 + cwd_size + 8 + 1));
	strcpy(*shell_path, "shell=");
	strcat(*shell_path, cwd);
	strcat(*shell_path, "/myshell");
	(*shell_path)[(6 + cwd_size + 8)] = '\0';

	putenv(*shell_path);
}

void set_parent_env()
{
	char * parent_path;

	int shell_size = strlen(getenv("shell"));
	parent_path = malloc(sizeof(char) * (7 + shell_size + 1));

	strcpy(parent_path, "parent=");
	strcat(parent_path, getenv("shell"));
	putenv(parent_path);
}

void sig_handler(int sig)
{
	if (sig == SIGCHLD) {
		int status;
		wait(&status);
		if (ampersand_flag == 1) {
			ampersand_flag = 0;
		}
	}
}

int is_all_whitespace(char * s)
{
	int i = 0;
	while (s[i] != '\0' && s[i] != '\n') {
		if (!is_whitespace(s[i])) {
				return 0;
		}
		i++;
	}
	return 1;
}
