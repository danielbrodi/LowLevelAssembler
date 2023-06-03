/*********************************FILE__HEADER*********************************\
* File:					macro.c
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/
#include "macro.h"
#include <stdlib.h>

/***************************** Global Definitions *****************************/

/************************* Functions  Implementations *************************/
Macro MacroCreate(char *name) {
    Macro *new_macro = (Macro *) malloc(sizeof(Macro));
    if (NULL == new_macro){
        return NULL;
    }
    new_macro->commands = (char **) malloc()
}
/******************************************************************************/
