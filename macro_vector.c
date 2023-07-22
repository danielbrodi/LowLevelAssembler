#include "macro_vector.h"

Vector *new_vector() {
    Vector *v = malloc(sizeof(Vector));
    v->capacity = INITIAL_CAPACITY;
    v->size = 0;
    v->commands = malloc(sizeof(char *) * v->capacity);
    return v;
}

void push_back(Vector *v, char *value) {
 char **new_commands = NULL;
    if (v->size == v->capacity) {
        v->capacity *= 2;
       new_commands = realloc(v->commands, sizeof(char *) * v->capacity);
        if (new_commands == NULL) {
            fprintf(stderr, "Error reallocating memory\n");
            /* Handle the error condition appropriately (e.g., return an error code) */
            return;
        }
        v->commands = new_commands;
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
