/*********************************FILE__HEADER*********************************\
* File:					my_set.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					April-2023			   								
* Description:			The API file that exposes the functions that a user can
 *                      use as part of the `my_set` program.
\******************************************************************************/
#ifndef ASSIGNMENT2_MY_SET_H
#define ASSIGNMENT2_MY_SET_H

/*  Struct definition for a Vector - a dynamically sized array of integers. */
typedef struct Vector Vector;

/**
 * Allocates memory and creates a new Vector.
 *
 * @return Pointer to the newly created Vector.
 * Returns NULL if memory allocation fails.
 */
Vector *create_vector();

/**
 * Frees the memory used by a Vector.
 *
 * @param vector Pointer to the Vector to free.
 */
void free_vector(Vector *vector);

/**
 * Prints the elements of a Vector to standard output.
 *
 * @param vector Pointer to the Vector to print.
 * If vector is NULL, an error message will be printed.
 */
void print_vector(Vector *vector);

/**
 * Function to get a set of integers from user input.
 * Creates, fills up and returns a Set which is essentially a vector but
 * without duplicates.
 * The function reads integers from standard input and adds them to a set.
 * Only adds integers as elements to the set if they are not already present.
 * Also fills up the input vector with the elements that are received from the user.
 *
 * @param user_input Pointer to the Vector that should contain the input elements.
 * @return Pointer to the created and filled up Set.
 * @return NULL if memory allocation fails for creating a new set,
 * or if user_input is NULL.
 */
Vector *get_set(Vector *user_input);

/**
 * Prints the contents of a Set to standard output.
 *
 * @param set Pointer to the Set vector.
 * If set is NULL, an error message will be printed.
 */
void print_set(Vector *set);

#endif
