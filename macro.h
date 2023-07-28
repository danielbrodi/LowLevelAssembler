/*********************************FILE__HEADER*********************************\
* File:					macro.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					July-2023			   								
* Description:			
\******************************************************************************/

#ifndef MACRO_H
#define MACRO_H

#include "vector.h"
#include "utils.h" /* my_strdup */

typedef struct {
    char *name;
    Vector *commands;
} Macro;

typedef struct {
    Macro **macros;
    int size;
    int capacity;
} MacroVector;

MacroVector *new_macro_vector();
void push_back_macro(MacroVector *mv, Macro *m);
Macro *new_macro(char *name);
void free_macro(Macro *m);

#endif

