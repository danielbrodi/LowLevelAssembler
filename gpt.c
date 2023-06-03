#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 2

char *my_strdup(const char *s);

typedef struct {
    char **commands;
    int size;
    int capacity;
} Vector;

typedef struct {
    char *name;
    Vector *commands;
} Macro;

typedef struct {
    Macro **macros;
    int size;
    int capacity;
} MacroVector;

Vector *new_vector() {
    Vector *v = malloc(sizeof(Vector));
    v->capacity = INITIAL_CAPACITY;
    v->size = 0;
    v->commands = malloc(sizeof(char *) * v->capacity);
    return v;
}

void push_back(Vector *v, char *value) {
    if (v->size == v->capacity) {
        v->capacity *= 2;
        v->commands = realloc(v->commands, sizeof(char *) * v->capacity);
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

char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *new_str = (char *) malloc(len);
    if (new_str == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }
    memcpy(new_str, s, len);
    return new_str;
}

int main() {
    /* If the line starts with a macro invocation, expand it */
    Macro *macroToExpand = NULL;
    int i;
    FILE *inputFile = fopen("prog.as", "r");
    FILE *outputFile = fopen("prog.am", "w");
    char line[1024];
    Macro *currentMacro = NULL;
    MacroVector *macroVector = new_macro_vector();

    /* First pass: build the list of macros */
    while (fgets(line, sizeof(line), inputFile)) {
        /* If the line starts a macro definition */
        if (strncmp(line, "mcro", 4) == 0) {
            /* Remove the newline at the end of the line, if present */
            if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
            /* Start a new macro definition */
            currentMacro = new_macro(line + 5);
            push_back_macro(macroVector, currentMacro);
        } else if (strncmp(line, "endmcro", 7) == 0) {
            /* End of the current macro definition */
            currentMacro = NULL;
        } else if (currentMacro) {
            /* Inside a macro definition: add the line to the macro's command list */
            char *command = my_strdup(line);
            size_t length = strlen(command);
            if (length > 0 && command[length - 1] == '\n') {
                command[length - 1] = '\0';
            }
            push_back(currentMacro->commands, command);
            free(command);
        }
    }

/* Rewind the input file to the beginning for the second pass */
    rewind(inputFile);

/* Second pass: output the file, expanding macros */
    while (fgets(line, sizeof(line), inputFile)) {
        /* Remove the newline at the end of the line, if present */
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        /* If the line starts with a macro definition or end of macro, skip it */
        if (strncmp(line, "mcro", 4) == 0) {
            while (0 != strncmp(line, "endmcro", 7)) {
                fgets(line, sizeof(line), inputFile);
            }
            continue;
        }

        /* Check if the line is a macro invocation */
        Macro *macroToExpand = NULL;
        for (i = 0; i < macroVector->size; ++i) {
            if (strcmp(line, macroVector->macros[i]->name) == 0) {
                macroToExpand = macroVector->macros[i];
                break;
            }
        }

        if (macroToExpand) {
            /* Expand the macro */
            for (i = 0; i < macroToExpand->commands->size; ++i) {
                fputs(macroToExpand->commands->commands[i], outputFile);
                fputs("\n", outputFile);
            }
        } else {
            /* Otherwise, write the line to the output as is */
            fputs(line, outputFile);
            fputs("\n", outputFile);
        }
    }


    fclose(inputFile);
    fclose(outputFile);
    for (i = 0; i < macroVector->size; ++i) {
        free_macro(macroVector->macros[i]);
    }
    free(macroVector->macros);
    free(macroVector);

    return 0;
}