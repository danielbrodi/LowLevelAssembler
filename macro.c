#include "macro.h"

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

