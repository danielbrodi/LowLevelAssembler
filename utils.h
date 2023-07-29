/*********************************FILE__HEADER*********************************\
* File:					utils.h
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			This header file provides the declarations for utility functions
*                       and type definitions for boolean and function return status.
\******************************************************************************/

#ifndef MAMAN14_UTILS_H
#define MAMAN14_UTILS_H

/**
 * An enum representing function return statuses.
 */
typedef enum {
    SUCCESS, /**< Represents a successful function execution */
    FAILURE /**< Represents a failed function execution */
} Status;

/**
 * An enum representing boolean values.
 */
typedef enum {
    FALSE, /**< Represents a boolean False value */
    TRUE /**< Represents a boolean True value */
} Boolean;

/**
 * Duplicates a string.
 *
 * @param s - The string to be duplicated.
 * @return - A pointer to the duplicated string.
 */
char *my_strdup(const char *s);

/**
 * Filters a line to determine if it is a comment or blank line.
 *
 * @param line - The line to be filtered.
 * @return - 1 if the line is a comment or blank line, 0 otherwise.
 */
int filter_line(const char *line);

/**
 * Converts all characters in a string to lowercase.
 *
 * @param str - The string to be converted.
 */
void to_lowercase(char *str);

/**
 * Checks if a string starts with a specific prefix.
 *
 * @param str - The string to be checked.
 * @param prefix - The prefix to be searched.
 * @return - 1 if the string starts with the prefix, 0 otherwise.
 */
int startsWith(const char *str, const char *prefix);

#endif