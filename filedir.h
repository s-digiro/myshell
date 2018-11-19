#ifndef FILEDIR_H
#define FILEDIR_H

/* Filename:    main.c
 * Author:      Sean DiGirolamo
 * Version:     1.0.0
 * Date:        03-07-18
 * Purpose:     Header file for filedir.c. Contains function definitions for
 *              functions used in myshell for the purpose of dealing with
 *              files or directories. Usage of each function can be found in
 *              filedir.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"

// Returns true if path exists as either a directory of a file
int exists(char * path);

// Returns true if file at filepath exists, false otherwise
int file_exists(char * filepath);

// Returns true if directory at dirpath exists, false otherwise
int dir_exists(char * dirpath);

// Searches for a path to filename and returns that path if found
// If filename begins with /, then it will look for an absolute path from the
// root directory
// If filename begins with a ., then it will look for the file relative to the
// current directory
// In all other cases, it will search for the file in the directories listed in
// the environment variable "PATH"
char * get_path(char * filename);

#endif
