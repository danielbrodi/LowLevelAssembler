/*********************************FILE__HEADER*********************************\
*
* File:                 macro.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          This file contains implementations of functions used to
*                       create, manipulate, and delete macros in the assembly
*                       language program.
*                       Macros are stored in a dynamic array (vector).
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdlib.h>

#include "macro.h"
#include "utils.h"  /* my_strdup function */

/************************* Functions  Implementations *************************/
/**
 * Creates a new MacroVector, allocates initial memory, and sets default values.
 *
 * @return - A pointer to the newly created MacroVector.
 *
 * The function allocates memory for a MacroVector struct and an array of Macro pointers,
 * sets the initial capacity and size, and returns a pointer to the new MacroVector.
 */
MacroVector *new_macro_vector() {
    MacroVector *mv = malloc(sizeof(MacroVector));
    mv->capacity = INITIAL_CAPACITY;
    mv->size = 0;
    mv->macros = malloc(sizeof(Macro *) * mv->capacity);
    return mv;
}
/******************************************************************************/
/**
 * Adds a new Macro to the end of a MacroVector.
 *
 * @param mv - A pointer to the MacroVector.
 * @param m - A pointer to the Macro to be added.
 *
 * If the MacroVector's current size has reached its capacity,
 * the function doubles the capacity by reallocating memory.
 * The Macro is then added to the end of the MacroVector.
 */
void push_back_macro(MacroVector *mv, Macro *m) {
    if (mv->size == mv->capacity) {
        mv->capacity *= 2;
        mv->macros = realloc(mv->macros, sizeof(Macro *) * mv->capacity);
    }
    mv->macros[mv->size++] = m;
}
/******************************************************************************/
/**
 * Creates a new Macro with the given name.
 *
 * @param name - The name to be assigned to the new Macro.
 * @return - A pointer to the new Macro.
 *
 * The function allocates memory for a Macro struct,
 * duplicates the provided name string and assigns it to the Macro,
 * initializes a new command vector for the Macro,
 * and returns a pointer to the new Macro.
 */
Macro *new_macro(char *name) {
    Macro *m = malloc(sizeof(Macro));
    m->name = my_strdup(name);
    m->commands = new_vector();
    return m;
}
/******************************************************************************/
/**
 * Frees all memory associated with a Macro, including its name and commands.
 *
 * @param m - A pointer to the Macro to be freed.
 *
 * The function frees the memory allocated for the Macro's name string,
 * then frees the command vector associated with the Macro,
 * and finally frees the memory allocated for the Macro struct itself.
 */
void free_macro(Macro *m) {
    free(m->name);
    free_vector(m->commands);
    free(m);
}
/******************************************************************************/
