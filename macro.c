/*********************************FILE__HEADER*********************************\
* File:					macro.c
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdlib.h>

#include "macro.h"
#include "utils.h"

/***************************** Global Definitions *****************************/
#define INITIAL_CAPACITY 4

/************************* Functions  Implementations *************************/
Vector *new_vector() {
    Vector *v = malloc(sizeof(Vector));
    v->capacity = INITIAL_CAPACITY;
    v->size = 0;
    v->commands = malloc(sizeof(char *) * v->capacity);
    return v;
}

void push_back(Vector *v, char *value) {
    if (v->size == v->capacity) {
        v->capacity *= 2;
        v->commands = (char **) realloc(v->commands,
                                        sizeof(char *) * v->capacity);
        /*TODO free pre-assigned memory if failed to realloc */
    }
    v->commands[v->size++] = my_strdup(value);
}

void free_vector(Vector *v) {
    int i;
    for (i = 0; i < v->size; ++i) {
        free(v->commands[i]);
    }
    free(v->commands);
    free(v);
}

MacroVector *new_macro_vector() {
    MacroVector *mv = malloc(sizeof(MacroVector));
    mv->capacity = INITIAL_CAPACITY;
    mv->size = 0;
    mv->macros = malloc(sizeof(Macro *) * mv->capacity);
    return mv;
}

void push_back_macro(MacroVector *mv, Macro *m) {
    if (mv->size == mv->capacity) {
        mv->capacity *= 2;
        mv->macros = realloc(mv->macros, sizeof(Macro *) * mv->capacity);
    }
    mv->macros[mv->size++] = m;
}

Macro *new_macro(char *name) {
    Macro *m = malloc(sizeof(Macro));
    m->name = my_strdup(name);
    m->commands = new_vector();
    return m;
}

void free_macro(Macro *m) {
    free(m->name);
    free_vector(m->commands);
    free(m);
}
/******************************************************************************/
