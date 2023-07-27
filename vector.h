/*********************************FILE__HEADER*********************************\
* File:					vector.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					July-2023			   								
* Description:			
\******************************************************************************/

/******************************** Header Files ********************************/

/***************************** Global Definitions *****************************/

/************************* Functions  Implementations *************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdio.h>

#define INITIAL_CAPACITY 4

typedef struct {
    void **items;
    int size;
    int capacity;
} Vector;

Vector *new_vector();
void push_back(Vector *v, void *value);
void free_vector(Vector *v);

#endif

