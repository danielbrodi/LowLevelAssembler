/*********************************FILE__HEADER*********************************\
* File:					macro_vector.h
* Author:				Daniel and lior 
* Date:					July-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_MACRO_VECTOR_H
#define MAMAN14_MACRO_VECTOR_H
#define INITIAL_CAPACITY 4

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

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
