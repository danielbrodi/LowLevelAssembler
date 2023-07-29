/*********************************FILE__HEADER*********************************\
* File:					vector.h
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			API of the Vector data structure.
\******************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

/***************************** Global Definitions *****************************/

/* Initial capacity of the vector */
#define INITIAL_CAPACITY 5

/* Vector struct definition */
typedef struct {
    void **items; /* Dynamic array of items */
    int size; /* Current size of the vector */
    int capacity; /* Current capacity of the vector */
} Vector;

/************************* Functions Declarations *************************/

/**
 * Creates a new vector.
 *
 * @return A pointer to the newly created vector.
 */
Vector *new_vector();

/**
 * Adds an element to the end of the vector.
 * In case of an memory allocation error the program will exit.
 *
 * @param v - The vector.
 * @param value - The value to be added.
 */
void push_back(Vector *v, void *value);

/**
 * Frees the memory used by the vector.
 *
 * @param v - The vector.
 */
void free_vector(Vector *v);

#endif