/*********************************FILE__HEADER*********************************\
* File:					gpt_daniel.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:					July-2023
* Description:
\*****************************************************************/

#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "program_constants.h"
#include "macro_processing.h"
#include "label_processing.h"
#include "am_file_processing.h"
#include "binary_and_ob_processing.h"
void freeProgramState(ProgramState *programState) {
    if (programState->labels != NULL) {
        free(programState->labels);
        programState->labels = NULL; /*Set the pointer to NULL to avoid dangling pointer*/
    }

    if (programState->externalLabels != NULL) {
        free(programState->externalLabels);
        programState->externalLabels = NULL; /*Set the pointer to NULL to avoid dangling pointer*/
    }
}
/******************************* Main  Function *******************************/
int main(int argc, char *argv[]) {
    ProgramState programState;
    Status stages_status = SUCCESS;
    int i = 0 /*j = 0*/;
    FILE *file = NULL;
    size_t argLength = strlen(argv[i]);

    char *file_name_as = NULL, *file_name_am = NULL, *file_name_ent = NULL,
            *file_name_ext = NULL, *file_name_bin = NULL, *file_name_ob = NULL;

    if (argc < 2) {
        printf("Please provide file names as command-line arguments.\n");
        return FAILURE;
    }
    for (i = 1; i < argc; i++) {
        /*printf("%s\n", argv[i]);*/
        /* Append extensions to the current file name */
        file_name_as = malloc(argLength + 4); /* 4 for ".as\0" */
        file_name_am = malloc(argLength + 4); /* 4 for ".am\0" */
        file_name_ent = malloc(argLength + 5); /* 5 for ".ent\0" */
        file_name_ext = malloc(argLength + 5); /* 5 for ".ext\0" */
        file_name_bin = malloc(argLength + 5); /* 5 for ".bin\0" */
        file_name_ob = malloc(argLength + 4); /* 4 for ".ob\0" */

        sprintf(file_name_as, "%s.as", argv[i]);
        sprintf(file_name_am, "%s.am", argv[i]);
        sprintf(file_name_ent, "%s.ent", argv[i]);
        sprintf(file_name_ext, "%s.ext", argv[i]);
        sprintf(file_name_bin, "%s.bin", argv[i]);
        sprintf(file_name_ob, "%s.ob", argv[i]);


        file = fopen(file_name_as, "r");
        if (file == NULL) {
            printf("Failed to open the file: %s\n", file_name_as);
            free(file_name_as);
            free(file_name_am);
            free(file_name_ent);
            free(file_name_ext);
            free(file_name_bin);
            free(file_name_ob);
            continue;  /* skip to the next file */
        }

        /* reset program state settings */
        programState.labels = NULL;         /* Initialize labels pointer to NULL */
        programState.externalLabels = NULL; /* Initialize externalLabels pointer to NULL */
        programState.label_count = 0;       /* Initialize other members as needed */
        programState.IC = 0;
        programState.DC = 0;
        programState.externalLabel_count = 0;
        programState.current_line_number = 100;

        /* start process */
        stages_status += preProcess(file_name_as, file_name_am, &programState);
        if (SUCCESS != stages_status) {
           remove(file_name_am);
           
        }
        else
        {
        stages_status += checkLabels(file_name_am, &programState);
        stages_status += ParseFile(file_name_am, file_name_bin, &programState);

        if (SUCCESS == stages_status) {
            WriteLabelsToFile(file_name_ent, file_name_ext, &programState);
            binaryToBase64(file_name_bin, file_name_ob, programState.IC,
                           programState.DC);
        }
        }
        /*for (j = 0; j < programState.label_count; j++) {
            printf("Label name: %s, line_number: %d, asm_line_number: %d.\n",
                   programState.labels[j].name,
                   programState.labels[j].line_number,
                   programState.labels[j].asm_line_number);

        }*/

        remove(file_name_bin);

        fclose(file);  /* Close the file when done */
        free(file_name_as);
        free(file_name_am);
        free(file_name_ent);
        free(file_name_ext);
        free(file_name_bin);
        free(file_name_ob);
        freeProgramState(&programState);

    }

    return 0;
}
