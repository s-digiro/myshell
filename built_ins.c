/* Filename:	built_ins.c
 * Author:	Sean DiGirolamo
 * Version:	1.0.0
 * Date:	03-07-18
 * Purpose:	Function definitions for functions used in myshell concerning
 *		built in commands. Usage instructions for each function can be
 *		found in built_ins.h
 */

#include "built_ins.h"
#include <stdlib.h>

int is_built_in(char * s)
{
        if (strcmp(s, "cd") == 0) {
                return 1;
        } else if (strcmp(s, "clr") == 0) {
                return 1;
        } else if (strcmp(s, "dir") == 0) {
                return 1;
        } else if (strcmp(s, "environ") == 0) {
                return 1;
        } else if (strcmp(s, "echo") == 0) {
                return 1;
        } else if (strcmp(s, "pause") == 0) {
                return 1;
        }

        return 0;
}

void built_ins(COM_SEG * seg)
{
        // cd, clr, dir, environ, echo, help, pause, quit
        char * s = seg->seg[0];

        if (strcmp(s, "cd") == 0) {
		// cd

                chdir(seg->seg[1]);

                exit(0);
        } else if (strcmp(s, "clr") == 0) {
		// clr

                for (int i = 0; i < 500; i++) {
                        printf("\n");
                }

                exit(0);
        } else if (strcmp(s, "dir") == 0) {
		// dir

                DIR *d;
                struct dirent *de;

		// Catches case where no directory is specified. Default to
		// current directory
		if (seg->seg[1] == NULL) {
			d = opendir(".");
		} else {
	                d = opendir(seg->seg[1]);
		}

		// Catches case where d does not exist
		if (d == NULL) {
			printf("Directory not found\n");
			exit(0);
		}

		// If d exists, print contents
                if (d) {
                        while ((de = readdir(d)) != NULL) {
                                printf("%s  ", de->d_name);
                        }
                        printf("\n");
                        closedir(d);
                }

                exit(0);
        } else if (strcmp(s, "environ") == 0) {
		// environ

                char *p = *environ;

                int i = 0;
		// Print each environ until NULL
                while (p != NULL) {
                        printf("%s\n", p);

                        p = *(environ + i);
                        i++;
                }

                exit(0);
        } else if (strcmp(s, "echo") == 0) {
		// echo

		// tracks whether or not a quote is active
                int quote_flag = 0;

		// Iterate through each string
                for (int i = 1; i < seg->strc; i++) {
			// Iterate through each character
                        for (int j = 0; j < strlen(seg->seg[i]); j++) {
                                char c = (seg->seg[i])[j];
				// check for quotes, but don't print them
                                if (c == '"') {
                                        if (quote_flag) {
                                                quote_flag--;
                                        } else {
                                                quote_flag++;
                                        }
				// Print everything else
                                } else {
                                        printf("%c", c);
                                }
                        }
			// Break if we see a space that isn't quoted
                        if (!quote_flag) {
                                break;
                        } else {
                                printf(" ");
                        }
                }
                printf("\n");

                exit(0);
        } else if (strcmp(s, "pause") == 0) {
		// pause

		// Just getchar from input and wait till that char is enter
                char x;
                while (x != '\n') {
                        x= getchar();
                }

                exit(0);
        }
}

// Giant ugly function for cd
void cd(COM_SEG * seg)
{
        char * newdir;
	char * newpwdenv;
        char c;		// Tracks which kind of cd we're doing ~/./// 
        int size;
        int new_size;
	char * dir;

	// Catches case where no arg is given (which will default to home dir)
        if (seg->strc == 1) {
		dir = malloc(sizeof(char) * (strlen(getenv("HOME")) + 1));
                strcpy(dir,getenv("HOME"));
		dir[strlen(getenv("HOME"))] = '\0';
        } else {
                dir = seg->seg[1];
        }

	c = dir[0];

        switch (c) {
        case '/' :
                // Absolute path

                newdir = dir;
                break;
        case '.' :
                // Current directory or previous

                if (dir[1] == '.') {
                        // Change to previous directory
                        size = strlen(getenv("PWD"));
                        newdir = malloc(sizeof(char) * (size + 1));
			strcpy(newdir, getenv("PWD"));
                        newdir[size] = '\0';
                        if (strcmp(newdir, "/") == 0) {
                                return;
                        }
                        int index = find_last(newdir, '/');
                        newdir[index] = '\0';

                        if (newdir[0] == '\0') {
                                newdir[0] = '/';
                                newdir[1] = '\0';
                        }
                }
		break;
        default :
                // Change relative to current directory

                size = strlen(getenv("PWD"));
                new_size = size + 1 + strlen(seg->seg[1]);
                newdir = malloc(sizeof(char) * (new_size + 1));
                strcpy(newdir, getenv("PWD"));
                strcat(newdir, "/");
                strcat(newdir, seg->seg[1]);
                newdir[new_size + 1] = '\0';

                break;
        }

        if (dir_exists(newdir)) {
                chdir(newdir);
		newpwdenv = malloc(sizeof(char) * (4 + strlen(newdir) + 1));
		strcpy(newpwdenv, "PWD=");
		strcat(newpwdenv, newdir);
		newpwdenv[4 + strlen(newdir)] = '\0';
                putenv(newpwdenv);
        } else {
                if (newdir[0] == newdir[1]) {
                        newdir = newdir + 1;
                }
                printf("Directory %s not found.\n", newdir);
        }

	if (c != '/' || seg->strc == 1){
		free(newdir);
	}
}

int find_last(char * s, char c)
{
        int index = -1;

        for (int i = 0; i < strlen(s); i++) {
                if (s[i] == c) {
                        index = i;
                }
        }

        return index;
}
