#include "label_processing.h"

Status checkLabels(char *am_file_name, ProgramState *programState) {
    int i = 0;
    char line[MAX_LINE_LENGTH] = {0};
    int line_number = 0;
    char new_label[MAX_LABEL_LENGTH] = {0};
    char *label_end = NULL;
    int label_length = -1;
    Status ret = SUCCESS;

    FILE *file = fopen(am_file_name, "r");

    if (!file) {
        printf("File not found: %s\n", am_file_name);
        return FAILURE;
    }

    /* First scan to validate and collect labels */
    while (fgets(line, sizeof(line), file)) {
        line_number++;

        /* Check if line starts with a label */
        label_end = strchr(line, ':');
        if (label_end) {
            /* Extract label */
            label_length = (int) (label_end - line);
            /* Check if label length is less than 31 */
            if (label_length >= MAX_LABEL_LENGTH) {
                PrintLabelErrorMessage(line_number, LABEL_LENGTH_EXCEEDS_LIMIT,
                                       NULL);
                ret = FAILURE;
            }

            /* Check if label is a saved word */
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';
            to_lowercase(new_label);

            for (i = 0; i < commandsListSize; i++) {
                if (strcmp(new_label, commandsList[i]) == 0) {
                    PrintLabelErrorMessage(line_number,
                                           LABEL_IS_RESERVED_COMMAND_WORD,
                                           new_label);
                ret = FAILURE;
                }
            }
      if (new_label[0] == '.') {
	    strcpy(new_label, new_label + 1);
	    for (i = 0; i < instructionsListSize; i++) {
                if (strcmp(new_label, instructionsList[i]) == 0) {
                    PrintLabelErrorMessage(line_number,
                                           LABEL_IS_RESERVED_INSTRUCTION_WORD,
                                           new_label);
                ret = FAILURE;
                }
            }
       }
       if (new_label[0] == '@') {
	    strcpy(new_label, new_label + 1);
            for (i = 0; i < registersListSize; i++) {
                if (strcmp(new_label, registersList[i]) == 0) {
                    PrintLabelErrorMessage(line_number,
                                           LABEL_IS_RESERVED_REGISTER_WORD,
                                           new_label);
                ret = FAILURE;
                }
            }
        }
      
            /* Check if label contains spaces */
            for (i = 0; i < label_length; i++) {
                if (isspace(line[i])) {
                    PrintLabelErrorMessage(line_number, INVALID_LABEL_FORMAT,
                                           new_label);
                ret = FAILURE;
                }
            }

            /* Check if the first character is a letter */
            if (!isalpha(new_label[0])) {
                PrintLabelErrorMessage(line_number,
                                       LABEL_MUST_START_WITH_LETTER, new_label);
                ret = FAILURE;
            }

            /* Check for duplicate labels */
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';
            for (i = 0; i < programState->label_count; i++) {
                if (strcmp(new_label, programState->labels[i].name) == 0) {
                    PrintLabelErrorMessage(line_number, DUPLICATE_LABEL,
                                           new_label);
                ret = FAILURE;
                }
            }

        /* Copy label to dynamically allocated labels array */
        programState->labels = realloc(programState->labels, (programState->label_count + 1) * sizeof(Label));
        if (programState->labels == NULL) {
            /*Handle memory allocation error*/
            fclose(file);
            return FAILURE;
        }

        strncpy(programState->labels[programState->label_count].name, new_label, label_length + 1);
        programState->labels[programState->label_count].line_number = line_number;
        programState->labels[programState->label_count].isEntry = 0;
        programState->labels[programState->label_count].isExtern = 0;
        programState->label_count++;
        } else {
            /* Check if line starts with an 'extern' instruction */
            if (startsWith(line, ".extern")) {
                /* Extract label */
                char *label_start = line + strlen(".extern") +
                                    1; /* +1 to skip the space after '.extern' */
                label_end = strpbrk(label_start,
                                    " \t\n"); /* find the end of the label */
                if (!label_end)
                    label_end = label_start +
                                strlen(label_start); /* end of string if no whitespace found */

                /* Check and add the label as before, but mark it as extern */
                label_length = (int) (label_end - label_start);
                strncpy(new_label, label_start, label_length);
                new_label[label_length] = '\0';

                /* Check for duplicate labels */
                for (i = 0; i < programState->label_count; i++) {
                    if (strcmp(new_label, programState->labels[i].name) == 0) {
                        PrintLabelErrorMessage(line_number, DUPLICATE_LABEL,
                                               new_label);
                ret = FAILURE;
                    }
                }
                
                 /* Copy label to dynamically allocated labels array */
        programState->labels = realloc(programState->labels, (programState->label_count + 1) * sizeof(Label));
        if (programState->labels == NULL) {
            /*Handle memory allocation error*/
            fclose(file);
            return FAILURE;
        }

        strncpy(programState->labels[programState->label_count].name, new_label, label_length + 1);
        programState->labels[programState->label_count].line_number = -1; /* No line number for extern labels */
        programState->labels[programState->label_count].isEntry = 0;
        programState->labels[programState->label_count].isExtern = 1;
        programState->label_count++;
            }
        }
    }

    fclose(file);

    /*
     for (i = 0; i < programState->label_count; i++) {
        printf("this is label %d: %s in line: %d\n", i,
               programState->labels[i].name,
               programState->labels[i].line_number);
    }
    */

    return ret;
}
/******************************************************************************/
void UpdateLines(char *words[], int num_of_words, int has_label,
                 ProgramState *programState) {
    ProgramState *currentProgramState = programState;

    int i, commandIdx, commandOrderInWords, operandIdx;
    char *command;
    int labelIdx;

    if (has_label) {
        commandOrderInWords = 1;
    } else {
        commandOrderInWords = 0;
    }

    command = words[commandOrderInWords];
    commandIdx = findCommand(command);

    if (has_label) {
        for (i = 0; i < currentProgramState->label_count; i++) {
            if (strcmp(words[0], currentProgramState->labels[i].name) == 0) {
                currentProgramState->labels[i].asm_line_number = currentProgramState->current_line_number;
                break;
            }
        }
    }

    if (commandIdx != -1) { /* If it's a command and not an instruction */
        for (operandIdx = commandOrderInWords + 1; operandIdx <=
                                                   commandOrderInWords +
                                                   paramCount[commandIdx]; operandIdx++) {
            if (isLabel(words[operandIdx], currentProgramState)) {
                labelIdx = getLabelIndex(words[operandIdx],
                                         currentProgramState);
                if (currentProgramState->labels[labelIdx].isExtern) {
                    if (has_label) {
                        if (operandIdx == 2) {
                            addExternalLabel(labelIdx,
                                             currentProgramState->current_line_number +
                                             1, currentProgramState);
                        } else {
                            addExternalLabel(labelIdx,
                                             currentProgramState->current_line_number +
                                             2, currentProgramState);
                        }
                    } else {
                        if (operandIdx == 1) {
                            addExternalLabel(labelIdx,
                                             currentProgramState->current_line_number +
                                             1, currentProgramState);
                        } else {
                            addExternalLabel(labelIdx,
                                             currentProgramState->current_line_number +
                                             2, currentProgramState);
                        }
                    }
                }
            }
        }
        currentProgramState->current_line_number +=
                paramCount[commandIdx] + 1;
        /* If both parameters are registers, we decrement the line count by 1 */
        if (paramCount[commandIdx] == 2 &&
            isValidParam(words[commandOrderInWords + 1],
                         OPERAND_TYPE_REGISTER, currentProgramState) &&
            isValidParam(words[commandOrderInWords + 2],
                         OPERAND_TYPE_REGISTER, currentProgramState)) {
            currentProgramState->current_line_number--;
        }
    } else { /* It's an instruction */
        if (strcmp(command, "string") == 0) {
            /* Including the null character at the end of the string */
            currentProgramState->current_line_number +=
                    strlen(words[commandOrderInWords + 1]) + 1;
        } else if (strcmp(command, "data") == 0) {
            if (has_label) {
                currentProgramState->current_line_number += num_of_words - 2;
            } else {
                currentProgramState->current_line_number += num_of_words - 1;
            }
        }
    }
}
/******************************************************************************/
void WriteLabelsToFile(const char *ent_filename, const char *ext_filename,
                       ProgramState *programState) {
    int i = 0;
    FILE *entry_fp = NULL;
    FILE *extern_fp = NULL;

    ProgramState *currentProgramState = programState;

    for (i = 0; i < currentProgramState->externalLabel_count; i++) {
        if (extern_fp == NULL) {
            extern_fp = fopen(ext_filename, "w");
            if (extern_fp == NULL) {
                printf("Unable to open external file (%s) for writing.\n",
                       ext_filename);
                return;
            }
        }
        fprintf(extern_fp, "%s %d\n",
                currentProgramState->externalLabels[i].name,
                currentProgramState->externalLabels[i].asm_line_number);
    }

    for (i = 0; i < currentProgramState->label_count; i++) {
        if (currentProgramState->labels[i].isEntry) {
            if (entry_fp == NULL) {
                entry_fp = fopen(ent_filename, "w");
                if (entry_fp == NULL) {
                    printf("Unable to open entry file (%s) for writing.\n",
                           ent_filename);
                    /* Close already opened extern file */
                    if (extern_fp != NULL) {
                        fclose(extern_fp);
                    }
                    return;
                }
            }
            fprintf(entry_fp, "%s %d\n", currentProgramState->labels[i].name,
                    currentProgramState->labels[i].asm_line_number);
        }
    }

    if (entry_fp != NULL) {
        fclose(entry_fp);
    }
    if (extern_fp != NULL) {
        fclose(extern_fp);
    }
}
/******************************************************************************/
void addExternalLabel(int labelIdx, int lineNumber, ProgramState *programState) {
    ProgramState *currentProgramState = programState;
    
    /* Dynamically allocate memory for the externalLabels array if it hasn't been allocated yet */
    if (currentProgramState->externalLabels == NULL) {
        currentProgramState->externalLabels = malloc(sizeof(Label));
        if (currentProgramState->externalLabels == NULL) {
            /* Handle memory allocation error */
            return;
        }
    } else {
        /* Resize the externalLabels array to accommodate the new label */
        Label* temp = realloc(currentProgramState->externalLabels, (currentProgramState->externalLabel_count + 1) * sizeof(Label));
        if (temp == NULL) {
            /* Handle memory reallocation error */
            return;
        }
        currentProgramState->externalLabels = temp;
    }

    /* Add the label index to the next position in the externalLabels array */
    currentProgramState->externalLabels[currentProgramState->externalLabel_count] = currentProgramState->labels[labelIdx];
    /* Add the line number to the lineNumber field */
    currentProgramState->externalLabels[currentProgramState->externalLabel_count].asm_line_number = lineNumber;
    /* Increment the counter for the next call */
    currentProgramState->externalLabel_count++;
}
/******************************************************************************/
