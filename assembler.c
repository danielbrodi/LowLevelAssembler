/*********************************FILE__HEADER*********************************
*
* File:                 assembler.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          The purpose of this program is to act as an assembler,
*                       converting assembly commands into their binary counterparts.
*                       The conversion process involves multiple stages, including
*                       preprocessing, label checking, parsing, and file writing.
*
*                       This program also generates various types of output files
*                       to provide a comprehensive view of the assembly-to-binary
*                       conversion process. These include a processed file (*.am),
*                       an entry labels file (*.ent), an extern labels file (*.ext),
*                       and a binary code file (*.ob).
*
*                       With a strong focus on error handling and memory management,
*                       this program ensures that all conversions are accurate and
*                       that all allocated memory is appropriately freed after use.
*
\******************************************************************************/

/******************************** Header Files ********************************/
#include <string.h>

#include "program_constants.h"
#include "macro_processing.h"
#include "label_processing.h"
#include "am_file_processing.h"
#include "binary_and_ob_processing.h"
#include "utils.h"

/**************************** Forward Declarations ****************************/
void initProgramState(ProgramState *);

void freeProgramState(ProgramState *programState);

void freeAllFiles(char *, char *, char *, char *, char *, char *);
/******************************************************************************/
/******************************* Main  Function *******************************/
/*
---------------------------------------------------------------------------------
    Serves as the program's entry point. The flow of the program includes:

    1.  Initiation of the program state.
        The assumption of the program is that the starting memory block is
        located at address '100'.
    2. Reading files and appending appropriate extensions(.am, .ext, .ent, .ob).
    3. Preprocessing: reading and expanding macros.
    4. Checking labels for any discrepancies.
    5. Parsing files to understand and validate the syntax.
    6. Writing all labels, both entry & external to seperated files(.ext, .ent).
    7. Conversion from binary to base64 and  save in a file (.ob).

    Note: If any of the stages encounters an error or fails, the program will
    not produce a base64 *.ob file.

    8. Finally, freeing the program state after processing.

Parameters:
    int argc - The number of command-line arguments.
    char *argv[] - An array of command-line arguments, which are expected
                   to be names of the files to be processed.

Returns:
    int - Returns 0 on successful execution.
---------------------------------------------------------------------------------
*/
int main(int argc, char *argv[]) {
    /* Initialization of structures and variables */
    ProgramState programState;
    Status stages_status = SUCCESS;
    FILE *file = NULL;
    int i = 0;

    char *file_name_as = NULL, *file_name_am = NULL, *file_name_ent = NULL,
            *file_name_ext = NULL, *file_name_bin = NULL, *file_name_ob = NULL;

    /* validate that at least one file for processing was provided */
    if (argc < 2) {
        printf("Please provide file names as command-line arguments.\n");
        return FAILURE;
    }

    for (i = 1; i < argc; i++) {
        size_t argLength = strlen(argv[i]);

        /* Memory allocation with validation */
        if ((file_name_as = malloc(argLength + 4)) == NULL ||
            /* 4 for ".as\0" */
            (file_name_am = malloc(argLength + 4)) == NULL ||
            /* 4 for ".am\0" */
            (file_name_ent = malloc(argLength + 5)) == NULL ||
            /* 5 for ".ent\0" */
            (file_name_ext = malloc(argLength + 5)) == NULL ||
            /* 5 for ".ext\0" */
            (file_name_bin = malloc(argLength + 5)) == NULL ||
            /* 5 for ".bin\0" */
            (file_name_ob = malloc(argLength + 4)) ==
            NULL) {   /* 4 for ".ob\0" */
            printf("Error: Memory allocation for info files was failed!\n");
            exit(1);
        }

        /* Construct file names with proper extensions */
        sprintf(file_name_as, "%s.as", argv[i]);
        sprintf(file_name_am, "%s.am", argv[i]);
        sprintf(file_name_ent, "%s.ent", argv[i]);
        sprintf(file_name_ext, "%s.ext", argv[i]);
        sprintf(file_name_bin, "%s.bin", argv[i]);
        sprintf(file_name_ob, "%s.ob", argv[i]);

        /* File opening with validation */
        if ((file = fopen(file_name_as, "r")) == NULL) {
            printf("Failed to open the file: %s\n", file_name_as);
            freeAllFiles(file_name_as, file_name_am, file_name_ent,
                         file_name_ext, file_name_bin, file_name_ob);
            continue;  /* Skip to the next file */
        }

        /* Initialize program state settings */
        initProgramState(&programState);

        /* Process the file */
        stages_status += preProcess(file_name_as, file_name_am, &programState);
        if (SUCCESS != stages_status) {
            remove(file_name_am);
        } else {
            stages_status += checkLabels(file_name_am, &programState);
            stages_status += ParseFile(file_name_am, file_name_bin,
                                       &programState);
            /** Only if all previous stages (pre process, syntax validation and parsing) were successful,
             * write labels to file and convert binary to Base64 */
            if (SUCCESS == stages_status) {
                WriteLabelsToFile(file_name_ent, file_name_ext, &programState);
                binaryToBase64(file_name_bin, file_name_ob, programState.IC,
                               programState.DC);
            }
        }

        remove(file_name_bin);
        fclose(file);
        freeAllFiles(file_name_as, file_name_am, file_name_ent, file_name_ext,
                     file_name_bin, file_name_ob);

        /* Free the program state */
        freeProgramState(&programState);
    }

    return 0;
}
/******************************************************************************/
/*
--------------------------------------------------------------------------------
    Initializes a ProgramState structure by allocating memory for the
    labels and externalLabels vectors and setting initial values for all members.

Parameters:
    ProgramState *programState - A pointer to the ProgramState structure to initialize.
--------------------------------------------------------------------------------
*/
void initProgramState(ProgramState *programState) {
    programState->labels = new_vector();
    programState->externalLabels = new_vector();
    programState->IC = 0;
    programState->DC = 0;
    programState->current_line_number = 100; /* assumed starting memory block address */
}
/******************************************************************************/
/*
--------------------------------------------------------------------------------
    Frees up allocated memory for labels and external labels,
    to prevent memory leaks and manage memory of dynamic memory allocated arrays
    in the program.

Parameters:
    ProgramState *programState - Pointer to the program state object to be freed.
--------------------------------------------------------------------------------
*/
void freeProgramState(ProgramState *programState) {
    if (programState->labels != NULL) {
        free_vector(programState->labels);
        programState->labels = NULL; /* Set the pointer to NULL to avoid dangling pointer */
    }

    if (programState->externalLabels != NULL) {
        free_vector(programState->externalLabels);
        programState->externalLabels = NULL; /* Set the pointer to NULL to avoid dangling pointer */
    }
}
/******************************************************************************/
/*
--------------------------------------------------------------------------------
    Frees all dynamically allocated memory used for file name strings.

Parameters:
    char *file_name_as  - Pointer to the string used for the ".as" file name.
    char *file_name_am  - Pointer to the string used for the ".am" file name.
    char *file_name_ent - Pointer to the string used for the ".ent" file name.
    char *file_name_ext - Pointer to the string used for the ".ext" file name.
    char *file_name_bin - Pointer to the string used for the ".bin" file name.
    char *file_name_ob  - Pointer to the string used for the ".ob" file name.
--------------------------------------------------------------------------------
*/
void freeAllFiles(char *file_name_as, char *file_name_am, char *file_name_ent,
                  char *file_name_ext, char *file_name_bin,
                  char *file_name_ob) {
    free(file_name_as);
    free(file_name_am);
    free(file_name_ent);
    free(file_name_ext);
    free(file_name_bin);
    free(file_name_ob);
}
/******************************************************************************/