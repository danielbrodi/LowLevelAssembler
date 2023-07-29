/*********************************FILE__HEADER*********************************\
* File:					vector.c
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			Implementation of a dynamic Vector data structure.
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdlib.h>
#include <stdio.h>

#include "vector.h"
/************************* Functions  Implementations *************************/
/**
 * Allocates and initializes a new Vector.
 *
 * The function creates a new vector by allocating memory for a Vector struct
 * and its items array, initializing its size to 0 and its capacity to the defined
 * INITIAL_CAPACITY. If memory allocation fails at any point, the function prints
 * an error message to stderr and exits with a status code of 1. If memory
 * allocation for the items array fails, the memory for the Vector struct is
 * also freed before the program exits.
 *
 * @return a pointer to the newly created Vector.
 */
Vector *new_vector() {
    /* Allocate memory for a new Vector */
    Vector *v = malloc(sizeof(Vector));

    /* Check if malloc failed */
    if (v == NULL) {
        fprintf(stderr, "Error allocating memory for vector\n");
        exit(1);
    }

    /* Initialize the Vector with initial capacity */
    v->capacity = INITIAL_CAPACITY;
    v->size = 0;

    /* Allocate memory for the items array */
    v->items = malloc(sizeof(char *) * v->capacity);

    /* Check if malloc failed */
    if (v->items == NULL) {
        fprintf(stderr, "Error allocating memory for vector items\n");
        free(v); /*  free v to avoid a memory leak */
        exit(1);
    }

    /* Return the newly created vector */
    return v;
}
/******************************************************************************/
/**
 * Adds an element to the end of the vector.
 *
 * @param v - The vector.
 * @param value - The value to be added.
 */
void push_back(Vector *v, void *value) {
    void **new_items = NULL;
    /* If the vector is full, double its capacity */
    if (v->size == v->capacity) {
        v->capacity *= 2;
        /* Reallocate memory for the items array */
        new_items = realloc(v->items, sizeof(void *) * v->capacity);
        /* If the reallocation failed, print an error message and exit */
        if (new_items == NULL) {
            fprintf(stderr, "Error reallocating memory for a vector!\n");
            exit(1);
        }
        v->items = new_items;
    }

    /* Add the new value to the vector */
    v->items[v->size++] = value;
}
/******************************************************************************/
/**
 * Frees the memory used by the vector.
 *
 * @param v - The vector.
 */
void free_vector(Vector *v) {
    int i = 0;
    /* Free each item in the items array */
    for (; i < v->size; ++i) {
        free(v->items[i]);
    }

    /* Free the items array and the vector itself */
    free(v->items);
    free(v);
}
/******************************************************************************/