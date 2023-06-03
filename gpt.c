#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"
#include "utils.h"

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
        if (1 == filter_line(line)) {
            continue;
        }
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
        if (1 == filter_line(line)) {
            continue;
        }
        /* Remove the newline at the end of the line, if present */
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        /* If the line starts with a macro definition or end of macro, skip it */
        if (strncmp(line, "mcro", 4) == 0) {
            while (0 != strncmp(line, "endmcro", 7)) {
                fgets(line, sizeof(line), inputFile);
            }
            continue;
        }

        macroToExpand = NULL;
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