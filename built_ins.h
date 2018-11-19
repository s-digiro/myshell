#ifndef BUILT_INS_H
#define BUILT_INS_H

/* Filename:	built_ins.h
 * Author:	Sean DiGirolamo
 * Version:	1.0.0
 * Date:	03-09-18
 * Purpose:	Header file for functions defined in built_in.c. Contains
 *		functions related to the built in commands used in myshell
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"
#include "filedir.h"
#include "built_ins.h"

/* checks if the COM_SEG is a builtin command, executes the builtin if it is */
void built_ins(COM_SEG * seg);

/* Function for the cd command. It is too complicated to be put in the built_ins
 * function
 */
void cd(COM_SEG * seg);

/* Returns true if the string is a built in command. False otherwise */
int is_built_in(char * s);

/* Returns index of last char c found in string s */
int find_last(char * s, char c);

extern char ** environ;

#endif
