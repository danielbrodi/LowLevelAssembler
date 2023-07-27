#include "macro_processing.h"

Status preProcess(const char *input_file, const char *output_file,
                  ProgramState *programState) {
    Status ret = SUCCESS;
    int i, count_line = 0;
    char line[1024];
    char *ptr = NULL, *newline = NULL, *end = NULL;
    FILE *inputFile = NULL, *outputFile = NULL;
    Macro *macroToExpand = NULL;
    Macro *currentMacro = NULL;
    MacroVector *macroVector = new_macro_vector();

    inputFile = fopen(input_file, "r");
    if (NULL == inputFile) {
        return FAILURE;
    }
    outputFile = fopen(output_file, "w");
    if (NULL == outputFile) {
        if (inputFile != NULL) {
            fclose(inputFile);
        }
        return FAILURE;
    }

    /* First pass: build the list of macros */
    while (fgets(line, sizeof(line), inputFile)) {
        count_line++;
        if (1 == filter_line(line)) {
            continue;
        }

        ptr = line;
        while (isspace((unsigned char) *ptr)) {
            ptr++;
        }

        if (strncmp(ptr, "mcro", 4) == 0) {
            ptr += 4;
            while (isspace((unsigned char) *ptr)) {
                ptr++;
            }
            end = ptr + strlen(ptr) - 1;
            while (end > ptr && isspace((unsigned char) *end)) {
                end--;
            }
            *(end + 1) = '\0';

            if (!isReservedKeyword(ptr, programState)) {
                if (!isValidMacroDefinition(
                        line)) {  /*Check validity of entire line*/
                    fprintf(stderr,
                            "Error: Invalid macro definition '%s' on line %d.\n",
                            line, count_line);
                    ret = FAILURE;
                } else {
                    currentMacro = new_macro(ptr);
                    push_back_macro(macroVector, currentMacro);
                }
            } else {
                fprintf(stderr, "Error: Invalid macro name '%s' on line %d.\n",
                        ptr, count_line);
                ret = FAILURE;
            }
        } else if (strncmp(ptr, "endmcro", 7) == 0) {
            ptr += 7;
            while (isspace((unsigned char) *ptr)) {
                ptr++;
            }
            if (*ptr != '\0' && *ptr != '\n' && *ptr != '\r') {
                fprintf(stderr,
                        "Error: Unexpected characters after 'endmcro' on line %d.\n",
                        count_line);
                ret = FAILURE;
            }
            currentMacro = NULL;
        } else if (currentMacro) {
            char *command = my_strdup(line);
            size_t length = strlen(command);
            if (length > 0 && command[length - 1] == '\n') {
                command[length - 1] = '\0';
            }
            push_back(currentMacro->commands, command);
        }
    }

/* Rewind the input file to the beginning for the second pass */
    rewind(inputFile);

/* Second pass: output the file, expanding macros */
    while (fgets(line, sizeof(line), inputFile)) {
        if (1 == filter_line(line)) {
            continue;
        }
        ptr = line;
        /* Skip leading whitespaces */
        while (isspace((unsigned char) *ptr)) {
            ptr++;
        }

        /* Remove the newline at the end of the line, if present */
        newline = strchr(ptr, '\n');
        if (newline) {
            *newline = '\0';
        }

        /* If the line starts with a macro definition or end of macro, skip it */
        if (strncmp(ptr, "mcro", 4) == 0) {
            do {
                fgets(line, sizeof(line), inputFile);
                ptr = line;
                while (isspace((unsigned char) *ptr)) {
                    ptr++;
                }
                newline = strchr(ptr, '\n');
                if (newline) {
                    *newline = '\0';
                }
            } while (strncmp(ptr, "endmcro", 7) != 0);
            continue;
        }

        macroToExpand = NULL;
        for (i = 0; i < macroVector->size; ++i) {
            /* If the line starts with a macro invocation, expand it */
            if (strncmp(ptr, macroVector->macros[i]->name,
                        strlen(macroVector->macros[i]->name)) == 0 &&
                (ptr[strlen(macroVector->macros[i]->name)] == ' ' ||
                 ptr[strlen(macroVector->macros[i]->name)] == '\n' ||
                 ptr[strlen(macroVector->macros[i]->name)] == '\0')) {
                macroToExpand = macroVector->macros[i];
                break;
            }
        }

        if (macroToExpand) {
            /* Expand the macro */
            for (i = 0; i < macroToExpand->commands->size; ++i) {
                char *expandedLine = macroToExpand->commands->items[i];
                /* Remove leading spaces from expanded line */
                while (isspace((unsigned char) *expandedLine)) {
                    expandedLine++;
                }
                fputs(expandedLine, outputFile);
                fputs("\n", outputFile);
            }
        } else {
            /* Otherwise, write the line to the output as is */
            fputs(ptr, outputFile);
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

    return ret;
}

/******************************************************************************/
int isReservedKeyword(char *word, ProgramState *programState) {
    if (findCommand(word) != -1) {
        return 1;
    }
    if (findInstruction(word) != -1) {
        return 1;
    }
    if (isRegister(word)) {
        return 1;
    }
    if (isLabel(word, programState)) {
        return 1;
    }
    return 0;
}

/******************************************************************************/
int isValidMacroDefinition(const char *line) {
    /* A valid macro definition has 2 space-separated words */
    int words = 0;
    while (*line) {
        /* Skip spaces */
        while (*line && isspace(*line)) line++;
        if (!*line) break;
        /* Found a word */
        words++;
        /* Skip the word */
        while (*line && !isspace(*line)) line++;
    }
    return (words == 2);
}
/******************************************************************************/
