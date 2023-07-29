/*********************************FILE__HEADER*********************************\
* File:					macro.h
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			This header file contains the definitions for the Macro
*                       and MacroVector structs and their related functions.
*                       These are used for handling and manipulating macros
*                       in assembly code processing.
\******************************************************************************/

#ifndef MACRO_H
#define MACRO_H

#include "vector.h" /* for Vector struct */

/**************************** Structs  Definitions ****************************/
/* A struct representing a Macro. It has a name and a list of commands. */
typedef struct {
    char *name;
    Vector *commands;
} Macro;

/** A struct representing a MacroVector.
 * It contains an array of Macro pointers and size information. */
typedef struct {
    Macro **macros;
    int size;
    int capacity;
} MacroVector;
/******************************************************************************/
/**
 * Creates a new MacroVector and allocates initial memory.
 * @return A pointer to the newly created MacroVector.
 */
MacroVector *new_macro_vector();
/**
 * Adds a new Macro to a MacroVector.
 * @param mv - A pointer to the MacroVector.
 * @param m - A pointer to the Macro to be added.
 */
void push_back_macro(MacroVector *mv, Macro *m);
/**
 * Creates a new Macro.
 * @param name - The name to be assigned to the new Macro.
 * @return A pointer to the new Macro.
 */
Macro *new_macro(char *name);
/**
 * Frees all memory associated with a Macro.
 * @param m - A pointer to the Macro to be freed.
 */
void free_macro(Macro *m);
/******************************************************************************/
#endif

