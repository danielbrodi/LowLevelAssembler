/*********************************FILE__HEADER*********************************\
* File:					utils.c
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:		    This file includes utility functions to handle
*                       common string operations, including duplication of
*                       strings, filtering of lines, conversion of strings
*                       to lowercase and checking if a string starts with
*                       a certain prefix.
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "utils.h"

/************************* Functions  Implementations *************************/
/**
 * Duplicates a given string.
 *
 * @param s - The string to be duplicated.
 *
 * @return A pointer to the newly allocated string which is a duplicate of the input string.
 */
char *my_strdup(const char *s) {
    /* Calculate the length of the string */
    size_t len = strlen(s) + 1;

    /* Allocate memory for the new string */
    char *new_str = (char *) malloc(len);
    if (new_str == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }

    /* Copy the original string into the new allocated memory */
    memcpy(new_str, s, len);
    return new_str;
}
/******************************************************************************/
/**
 * Filters out comment and blank lines.
 *
 * @param line - The line to be checked.
 *
 * @return 1 if the line is a comment or blank line, 0 otherwise.
 */

int filter_line(const char *line) {
    /* Remove leading spaces */
    char start = line[0];

    /* Check if the line is a comment or blank line */
    if (start == ';' || start == '\n') {
        return 1;
    }

    /* If it's not, it's a valid line */
    return 0;
}
/******************************************************************************/
/**
 * Converts a string to lowercase.
 *
 * @param str - The string to be converted.
 *
 * This function iterates over each character in the string and converts it to lowercase.
 */

void to_lowercase(char *str) {
    int i = 0;
    /* Iterate through each character in the string */
    for (; str[i]; i++) {
        /* Convert each character to lowercase */
        str[i] = tolower(str[i]);
    }
}
/******************************************************************************/
/**
 * Checks if a string starts with a certain prefix.
 *
 * @param str - The string to be checked.
 * @param prefix - The prefix to be searched for.
 *
 * @return 1 if the string starts with the prefix, 0 otherwise.
 */

int startsWith(const char *str, const char *prefix) {
    /* Compare the prefix length of the string with the prefix */
    return strncmp(str, prefix, strlen(prefix)) == 0;
}
/******************************************************************************/