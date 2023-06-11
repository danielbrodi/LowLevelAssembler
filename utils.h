/*********************************FILE__HEADER*********************************\
* File:					utils.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/

/***************************** Global Definitions *****************************/

/************************* Functions  Implementations *************************/

/******************************************************************************/
#ifndef MAMAN14_UTILS_H
#define MAMAN14_UTILS_H

typedef enum {
    SUCCESS,
    FAILURE
} Status;

typedef enum {
    FALSE,
    TRUE
} Boolean;

char *my_strdup(const char *s);

int filter_line(const char *line);

#endif
