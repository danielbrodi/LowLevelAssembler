/******************************** Header Files ********************************/
#include <stdio.h>
#include <stdlib.h>

/***************************** Global Definitions *****************************/
/* Defines the initial capacity of each created vector (dynamic array). */
#define INITIAL_CAPACITY 5

/** Defines a macro to calculate the enlarged size of the vector,
 * in case that a resize is required */
#define ENLARGE_SIZE(x) ((x) + INITIAL_CAPACITY)

/* Defines an enum to represent an operation outcome. */
typedef enum {
    SUCCESS, /* Value to indicate success of the operation. */
    FAILURE  /* Value to indicate failure of the operation. */
} op_status;

/* Defines an enum to represent boolean values. */
typedef enum {
    FALSE,
    TRUE
} boolean;

/* Vector data structure for storing a dynamically sized array of integers. */
typedef struct {
    char **elements; /* Pointer to the array of elements in the vector. */
    int size;      /* Current number of elements in the array. */
    int capacity;  /* Max number of elements that can be stored in the array. */
} Vector;

/************************* Functions  Implementations *************************/
/**
 * Function to create a new vector dynamically and initialize it with an
 * pre defined initial capacity.
 *
 * Returns:
 *  - Vector*: a pointer to the newly created vector
 *  - NULL: in case of a failure to allocate memory for the vector or
 *  the elements array
 */
Vector *create_vector() {
    /* Allocate memory for the vector structure */
    Vector *new_vector = (Vector *) malloc(sizeof(Vector));
    /* Check if allocation was successful, return null in case of failure */
    if (NULL == new_vector) {
        fprintf(stderr, "Failed to allocate memory for the vector\n");
        return (NULL);
    }

    /* Set initial capacity and current size of a new made vector */
    new_vector->capacity = INITIAL_CAPACITY;
    new_vector->size = 0;

    /* Allocate memory for the elements array in the vector */
    new_vector->elements = (char **) malloc(new_vector->capacity * sizeof(char *));
    /*  Check if allocation was successful, return null in case of a failure */
    if (NULL == new_vector->elements) {
        fprintf(stderr,
                "Failed to allocate memory for the elements array in the vector\n");
        /* Free the previously allocated memory for the vector */
        free(new_vector);
        return (NULL);
    }

    return new_vector;
}
/******************************************************************************/
/**
 * Function to resize (enlarge) the capacity of a vector dynamically.
 * The resize uses the `ENLARGE_SIZE` macro in order to enlarge by a fixed size
 * each needed time the dynamic array that contains the elements.
 *
 * Parameters:
 *  - vector: a pointer to the vector that needs to be resized.
 * Returns:
 *  - op_status: enum indicating(SUCCESS/FAILURE) whether the resize operation
 *  was successful or not.
 */
op_status resize_vector(Vector *vector) {
    op_status is_resized = FAILURE;
    /* enlarge capacity and calculate new capacity after resize */
    int new_capacity = ENLARGE_SIZE(vector->capacity);

    /* resize the elements array using realloc */
    char **resized_array = realloc(vector->elements, new_capacity * sizeof(char *));
    /* check if reallocation was successful */
    if (NULL != resized_array) {
        vector->elements = resized_array; /* update vector elements to the resized array */
        vector->capacity = new_capacity; /* update vector capacity to new capacity */
        resized_array = NULL;
        is_resized = SUCCESS; /* set operation status to success */
    }

    return is_resized;
}
/******************************************************************************/
/**
 * Function to check if a given value is present in a vector.
 * Iterates over the elements of the vector and searches for the
 * received element.
 *
 * Parameters:
 *  - vector: a pointer to the vector to be searched.
 *  - value: an integer value to search for in the vector.
 * Returns:
 *  - boolean: TRUE if the value is found, FALSE otherwise.
 */
boolean is_value_in_vector(Vector *vector, int value) {
    int i = 0;
    int size = vector->size;

    while (i < size) {
        if (vector->elements[i] == value) {
            return TRUE;
        }
        ++i;
    }

    return FALSE;
}
/******************************************************************************/
/**
 * Function to add a new element to the end of a given vector.
 * If the size of the vector is equal to its capacity,
 * which indicates that the vector is full and there is no space to add more
 * elements, the function first resizes the vector, increasing the capacity,
 * and only then adds the element.
 *
 * Parameters:
 *  - vector: a pointer to the vector to which an element will be added.
 *  - value: the value of the element to be added.
 * Returns:
 *  - op_status: enum indicating(SUCCESS/FAILURE) whether the add operation
 *  was successful or not.
 */
op_status add_to_vector(Vector *vector, int value) {
    op_status is_successfully_resized = SUCCESS;
    /*  Get the first available index for the new element */
    int first_available_idx = vector->size;

    /* Check if the vector has enough capacity to add a new element */
    if (vector->size == vector->capacity) {
        /* Resize the vector if the capacity is not enough */
        is_successfully_resized = resize_vector(vector);
    }

    /* If the vector is successfully resized, add the new element */
    if (SUCCESS == is_successfully_resized) {
        vector->elements[first_available_idx] = value;
        vector->size++;
    }

    return is_successfully_resized;
}
/******************************************************************************/
/**
 * Function to add an element to a set represented as a vector.
 * The function first checks if the value is already in the set,
 * if not, it uses the add_to_vector function to add the value to the set.
 *
 * Parameters:
 *  - set: a pointer to the vector representing the set.
 *  - value: the value to be added to the set.
 * Returns:
 *  - op_status: enum indicating(SUCCESS/FAILURE) whether the add operation
 *  was successful or not.
 */
op_status add_to_set(Vector *set, int value) {
    op_status is_successfully_added = SUCCESS;

    /*  Check if the value is already in the set */
    if (!is_value_in_vector(set, value)) {
        /* Add the value to the set using the add_to_vector function */
        is_successfully_added = add_to_vector(set, value);
    }

    return is_successfully_added;
}
/******************************************************************************/
/**
 * Function to free memory allocated for the vector structure and
 * its elements array.
 *
 * Parameters:
 *  - vector: a pointer to the vector to be freed.
 */
void free_vector(Vector *vector) {
    free(vector->elements);
    vector->elements = NULL;
    vector->size = 0;
    vector->capacity = 0;

    free(vector);
}
/******************************************************************************/
/**
 * Function to get a set of integers from user input.
 * The function reads integers from standard input and adds them to a vector and a set.
 * If the user input is NULL or there is a memory allocation error, NULL is returned.
 *
 * Parameters:
 *  - user_input: a pointer to the vector to which the user inputs will be added.
 * Returns:
 *  - Vector*: a pointer to the set of unique integers read from the user input.
 *            If there is a memory allocation error, NULL is returned.
 */
Vector *get_set(Vector *user_input) {
    int value_to_add;
    op_status is_added_to_set = SUCCESS;
    op_status is_added_to_vector = SUCCESS;

    /* Create a new set vector */
    Vector *my_set = create_vector();

    if (NULL != user_input && NULL != my_set) {
        printf("\nPlease enter integers in order to group them in a set.\n"
               "(press Ctrl+D to end):\n");

        /* Read user input until end of file (Ctrl+D) */
        while (scanf("%d", &value_to_add) != EOF) {
            /* Add value to user_input vector */
            is_added_to_vector = add_to_vector(user_input, value_to_add);
            /* Add value to my_set vector */
            is_added_to_set = add_to_set(my_set, value_to_add);

            /* Print error message if adding value to either vector failed */
            if (FAILURE == is_added_to_vector || FAILURE == is_added_to_set) {
                printf("\nFailed to add '%d' to the set, please try again.\n",
                       value_to_add);
            }
        }
    }

    return my_set;
}
/******************************************************************************/
/**
 * Function to print the elements in a given vector.
 *
 * Parameters:
 *  - vector: a pointer to the vector whose elements will be printed.
 *
 * Prints:
 *  - The elements in the vector, separated by a space.
 *  - If the vector pointer is NULL, an error message is printed.
 */
void print_vector(Vector *vector) {
    if (NULL != vector) {
        int size = vector->size;
        int i = 0;
        for (; i < size; i++) {
            printf("%d ", vector->elements[i]);
        }
    } else {
        fprintf(stderr, "Failed to print the vector because it's NULL.\n");
    }

    printf("\n");
}
/******************************************************************************/
/**
 * Function to print the contents of a given set.
 * The function calls the print_vector function to print the elements
 * of the set in the order they were added.
 *
 * Parameters:
 *  - set: a pointer to the set to be printed.
 *
 *  Prints:
 *  - The elements in the set, separated by a space.
 *  - If the set pointer is NULL, an error message is printed.
 */
void print_set(Vector *set) {
    if (NULL != set) {
        print_vector(set);
    } else {
        fprintf(stderr, "Failed to print the set because it's NULL.\n");
    }
}
/******************************************************************************/