/*********************************FILE__HEADER*********************************\
* File:					macro.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/

/***************************** Global Definitions *****************************/

/************************* Functions  Implementations *************************/

/******************************************************************************/
#ifndef MAMAN14_MACRO_H
#define MAMAN14_MACRO_H

typedef struct {
    char **commands;
    int size;
    int capacity;
} Vector;

typedef struct {
    char *name;
    Vector *commands;
} Macro;

typedef struct {
    Macro **macros;
    int size;
    int capacity;
} MacroVector;

Vector *new_vector();

void push_back(Vector *v, char *value);

void free_vector(Vector *v);

MacroVector *new_macro_vector();

void push_back_macro(MacroVector *mv, Macro *m);

Macro *new_macro(char *name);

void free_macro(Macro *m);

#endif
