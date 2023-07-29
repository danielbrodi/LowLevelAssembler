/*********************************FILE__HEADER*********************************\
* File:                 macro_processing.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          This source file contains the functionality for
*                       managing macro constructs in the our compiler program.
*                       It implements procedures for recognizing reserved
*                       keywords, validating macro definitions and performing
*                       a pre-processing step on assembly language input files.
*                       This preprocessing step identifies macro definitions,
*                       expands macros at their invocation, and writes the
*                       transformed code to an output file.
*                       These routines contribute to the larger task of
*                       parsing and interpreting the assembly language.
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macro_processing.h"
#include "macro.h"
#include "param_validation.h"

/**************************** Forward Declarations ****************************/
int isReservedKeyword(char *word, ProgramState *programState);

int isValidMacroDefinition(const char *line);

/************************* Functions  Implementations *************************/
/**
 * Preprocesses the given input file and writes the result to the output file.
 * Preprocessing involves expanding any macros defined in the input file.
 *
 * @param input_file The name of the file to preprocess.
 * @param output_file The name of the file to write the preprocessed code to.
 * @param programState The current state of the program.
 *
 * @return SUCCESS if the preprocessing is successful, FAILURE otherwise.
 */
Status preProcess(const char *input_file, const char *output_file,
                  ProgramState *programState) {
    /* Variable Initializations */
    Status ret = SUCCESS;
    int i, count_line = 0;
    char line[1024];
    char *ptr = NULL, *newline = NULL, *end = NULL;
    FILE *inputFile = NULL, *outputFile = NULL;
    Macro *macroToExpand = NULL;
    Macro *currentMacro = NULL;
    MacroVector *macroVector = new_macro_vector();

    /* Open input and output files. If opening fails, return FAILURE */
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
        /* Filter out comments and empty lines */
        if (1 == filter_line(line)) {
            continue;
        }
        /* Remove leading whitespaces */
        ptr = line;
        while (isspace((unsigned char) *ptr)) {
            ptr++;
        }

        /* Check for macro definitions */
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
        /* Iterate through all defined macros */
        for (i = 0; i < macroVector->size; ++i) {
            /** If the current line starts with the name of a macro, this indicates
             * a macro invocation. The macro name is followed by either a space,
             * newline or null character.
             *
             * strncmp is used to compare the first n characters of ptr and macro name,
             * where n is the length of the macro name. If they are equal, strncmp returns 0.
             *
             * If such a macro invocation is detected, the pointer macroToExpand is set to the
             * current macro, and the loop is exited early.
             */
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

    /* Cleanup: Close files and free allocated memory */
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
/**
 * This function checks if a given word is a reserved keyword in the assembly language.
 *
 * @param word - the word to be checked.
 * @param programState - the current state of the program.
 *
 * @return 1 if the word is a reserved keyword; 0 otherwise.
 *
 * The function checks if the word is a command, an instruction, a register, or a label.
 * If any of these checks is true, the function returns 1, indicating that the word is a reserved keyword.
 * If none of the checks is true, the function returns 0.
 */
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
/**
 * This function checks if a given line is a valid macro definition in the assembly language.
 *
 * @param line - the line to be checked.
 *
 * @return 1 if the line is a valid macro definition; 0 otherwise.
 *
 * A valid macro definition must have exactly two space-separated words.
 * The function counts the number of words in the line and returns 1 if there are exactly two words.
 * If the number of words is not two, the function returns 0.
 */
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
