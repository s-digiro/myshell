/* Filename:    filedir.c
 * Author:      Sean DiGirolamo
 * Version:     1.0.0
 * Date:        03-07-18
 * Purpose:	Contains function definitions for functions used in myshell for
 *		the purpose of dealing with files or directories. Usage of each
 *		function can be found in filedir.h
 */

#include "filedir.h"

// Path or file exists
int exists(char * path)
{
	if (file_exists(path) || dir_exists(path)) {
		return 1;
	}
	return 0;
}

int file_exists(char * filepath)
{
	if (filepath == NULL) {
		return 0;
	}
        if (access(filepath, F_OK) != -1) {
                return 1;
        }
        return 0;
}

int dir_exists(char * dirpath)
{
        DIR* dir = opendir(dirpath);
        if (dir) {
            closedir(dir);
            return 1;
        }
        return 0;
}

char * get_path(char * filename)
{
        char * filepath = NULL;
        int size;

        switch (filename[0]) {
        case '/' :
                // Absolute path name (Path name literal)

                // Create new string, copy path to string, set last bit to NULL
                size = strlen(filename);
                filepath = malloc(sizeof(char) * (size + 1));
                strcpy(filepath, filename);
                filepath[size] = '\0';
                if (exists(filepath)) {
                        return filepath;
                }
                break;
        case '.' :
                // Current directory

                // Create new string, append pwd, append filename (except .)
                // Last bit should be automatically set to NULL in strcat
                size = strlen(getenv("PWD"));
                filepath = malloc(sizeof(char) * (size + 2));
                strcpy(filepath, getenv("PWD"));
                filepath[size + 1] = '\0';
                filepath = strcat(filepath, filename + 1);
                if (exists(filepath)) {
                        return filepath;
                }
                break;
        default :
                // Search PATH

                // Get PATH and turn it into a null terminated string
                size = strlen(getenv("PATH"));
                char * path = malloc(sizeof(char) * (size + 1));
                strcpy(path, getenv("PATH"));
                path[size] = '\0';

                // Parse PATH to seperate each directory
                char ** dir;
                int dirc = parse_line(&dir, path, ':');

                // Path string is no longer needed
                free(path);

                // Check if file is in any directories in path
                for (int i = 0; i < dirc; i++) {
                        // Create hypothetical path
                        size = strlen(dir[i]) + 1 + strlen(filename);
                        filepath = malloc(sizeof(char) * (size + 1));
                        strcpy(filepath, dir[i]);
                        strcat(filepath, "/");
			strcat(filepath, filename);
                        filepath[size] = '\0';

                        // Check if path exists
                        if (exists(filepath)) {
                                // return if it exists
				int i = 0;
				while (dir[i] != NULL) {
					free(dir[i]);
					i++;
				}
				free(dir);
                                return filepath;
                        } else {
                                // Otherwise, free and continue loop
                                // Set to NULL in case it is not found
                                free(filepath);
                                filepath = NULL;
                        }
                }
		int i = 0;
		while (dir[i] != NULL) {
			free(dir[i]);
			i++;
		}
		free(dir);

                break;
        }

        // Return NULL if path has not been found
        return NULL;
}
