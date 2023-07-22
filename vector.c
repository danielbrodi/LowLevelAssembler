/*********************************FILE__HEADER*********************************\
* File:					vector.c
* Author:				Daniel Brodsky			 		  												  								
* Date:					July-2023			   								
* Description:			
\******************************************************************************/

#include "vector.h"
#include "utils.h" /* my_strdup */

Vector *new_vector() {
    Vector *v = malloc(sizeof(Vector));
    v->capacity = INITIAL_CAPACITY;
    v->size = 0;
    v->items = malloc(sizeof(char *) * v->capacity);
    return v;
}

void push_back(Vector *v, char *value) {
    char **new_items = NULL;
    if (v->size == v->capacity) {
        v->capacity *= 2;
        new_items = realloc(v->items, sizeof(char *) * v->capacity);
        if (new_items == NULL) {
            fprintf(stderr, "Error reallocating memory\n");
            return;
        }
        v->items = new_items;
    }

    v->items[v->size++] = my_strdup(value);
}

void free_vector(Vector *v) {
    int i;
    for (i = 0; i < v->size; ++i) {
        free(v->items[i]);
    }
    free(v->items);
    free(v);
}
