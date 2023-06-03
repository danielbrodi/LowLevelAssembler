/*********************************FILE__HEADER*********************************\
* File:					utils.c
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
/***************************** Global Definitions *****************************/

/************************* Functions  Implementations *************************/
char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *new_str = (char *) malloc(len);
    if (new_str == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }
    memcpy(new_str, s, len);
    return new_str;
}

int filter_line(const char *line) {
    /* Remove leading spaces */
    char start = line[0];
    /* Check if the line is a comment or blank line */
    if (start == ';' || start == '\n' || start == '\t') {
        return 1;
    }

    /* If it's not, it's a valid line */
    return 0;
}
/******************************************************************************/
