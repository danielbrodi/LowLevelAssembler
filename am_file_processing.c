/*********************************FILE__HEADER*********************************
*
* File:                 am_file_processing.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          This file contains the function for parsing and processing
*                       the .am files.
*                       It includes functionality for reading the file,
*                       handling different line states and writing to a binary file.
*
\******************************************************************************/

/******************************** Header Files ********************************/
#include "am_file_processing.h"

/******************************************************************************/
/*
--------------------------------------------------------------------------------
    ParseFile function processes the .am file, reads each line,
    and accordingly performs different actions based on the current state of each line.
    It writes results into a binary file.

Parameters:
    char *am_file_name - The name of the .am file to be parsed.
    char *bin_file_name - The name of the binary file to be written to.
    ProgramState *programState - Pointer to the program state object.

Return:
    Status - Returns SUCCESS if the file is successfully parsed and processed,
    FAILURE otherwise.
--------------------------------------------------------------------------------
 */
Status
ParseFile(char *am_file_name, char *bin_file_name, ProgramState *programState) {
    /* Variable declarations and initializations */

    /** The return status of the function,
     * which indicates every line in the file was processed as needed */
    Status ret = SUCCESS;

    /* File pointers for the assembly and binary files */
    FILE *file = NULL;
    FILE *bin_fp = NULL;

    /* Buffer to hold each line read from the assembly file */
    char buffer[80];

    /* Array of words parsed from each line of the assembly file */
    char *input_words[80];

    /* Representation of a single line from the assembly file */
    Line *new_line = NULL;

    /* Current line number in the assembly file, initialized to 1 (first line) */
    int line_number = 1;

    /* Number of words in the current line, initialized to 0 */
    int num_of_words = 0;

    /* Loop counters */
    int i = 0, j = 0;

    /* Flag indicating if a label exists in the current line, initialized to 0 (no label) */
    int has_label = 0;

    /* State of the line parser, initialized to -1 (undefined state) */
    State state = -1;

    /* Length of the current line or word, initialized to -1 */
    size_t len = -1;

    /* The last non-space character in the current line, initialized to '\0' */
    char last_non_space = '\0';

    /* File operations */
    /* Open binary file for writing */
    bin_fp = fopen(bin_file_name, "w");

    /* Open assembly file for reading */
    file = fopen(am_file_name, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return FAILURE;
    }


    /* Main loop - reads each line from the input file */
    while (fgets(buffer, sizeof(buffer), file)) {
        /* Remove newline character at the end of the buffer, if exists */
        len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        /* Skip quotes */
        for (i = 0, j = 0; i < len; i++) {
            if (buffer[i] != '"') {
                buffer[j++] = buffer[i];
            }
        }
        buffer[j] = '\0';
        num_of_words = 0;
        state = IN_LABEL_OR_COMMAND; /* starting state of each line */
        has_label = 0; /* A flag indicating if a label exists */
        input_words[(num_of_words)++] = buffer;

        /* Parse the line according to its current state */
        for (i = 0; buffer[i] != '\0'; i++) {
            char c = buffer[i];
            if (!isspace(c)) {
                last_non_space = c;
            }
            /** This switch-case statement handles the parsing of each character in a line read from the assembly file.
            *  It operates on different states which correspond to the different parts of a line,
            *  e.g., label, command, operand, etc.
            *  The state changes based on the type of the current character (e.g., alnum, space, comma, etc.) and the current state.
            *  The result of the parsing process is stored in input_words array.
            */
            switch (state) {
                /* The IN_LABEL_OR_COMMAND state processes a part of the line that could either be a label or a command: */
                case IN_LABEL_OR_COMMAND:
                    /** Check if the character is a space, a comma, or a colon.
                    *  If it's a space, mark the end of the current word and transition to AFTER_LABEL_OR_COMMAND state.
                    *  If it's a comma, handle it as an error since commas are not allowed in this context.
                    *  If it's a colon, mark that a label is present, mark the end of the label, and transition to AFTER_LABEL state.
                    */
                    if (isspace(c)) {
                        buffer[i] = '\0';
                        state = AFTER_LABEL_OR_COMMAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number, ILLEGAL_COMMA,
                                               ' ');
                        ret = FAILURE;
                        continue;
                    } else if (c == ':') {
                        has_label = 1;  /* Mark that a label is present */
                        buffer[i] = '\0';
                        state = AFTER_LABEL;
                    }
                    break;
                case AFTER_LABEL:
                    /**
                     * Check if we're in a command after a label. If so, add the command
                     * to the input words. If we encounter a comma, it's an error because
                     * commas are not expected directly after a label.
                     */
                    if (isalnum(c)) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_COMMAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number, ILLEGAL_COMMA,
                                               ' ');
                        ret = FAILURE;
                        continue;  /* Continue with next character */
                    }
                    break;
                    /* The AFTER_LABEL_OR_COMMAND state is entered after a label or command has been processed: */
                case AFTER_LABEL_OR_COMMAND:
                    /** If a label has been identified and the character is alphanumeric, start a new word (command) and transition to IN_COMMAND state.
                    *  If a label has not been identified and the character is alphanumeric, or '@', '-', '+', start a new word (operand) and transition to IN_OPERAND state.
                    *  If the character is a comma, handle it as an error since commas are not allowed in this context.
                    */
                    if (has_label && isalnum(c)) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_COMMAND;
                    } else if (!has_label &&
                               (isalnum(c) || c == '@' || c == '-' ||
                                c == '+')) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number, ILLEGAL_COMMA,
                                               ' ');
                        ret = FAILURE;
                        continue;  /* Continue with next character */
                    }
                    break;
                    /* The IN_COMMAND state processes the command part of the line (after a label): */
                case IN_COMMAND:
                    /** If the character is a space, mark the end of the current word and transition to AFTER_COMMAND state.
                    *  If the character is a comma, handle it as an error since commas are not allowed in this context.
                    */
                    if (isspace(c)) {
                        buffer[i] = '\0';
                        state = AFTER_COMMAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number, ILLEGAL_COMMA,
                                               ' ');
                        ret = FAILURE;
                        continue;
                    }
                    break;
                    /* The AFTER_COMMAND state is entered after a command has been processed: */
                case AFTER_COMMAND:
                    /** If the character is alphanumeric, or '@', '-', '+', start a new word (operand) and transition to IN_OPERAND state.
                    *  If the character is a comma, handle it as an error since commas are not allowed in this context.
                    */
                    if (isalnum(c) || c == '-' || c == '+' || c == '@') {
                        input_words[(num_of_words)++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number, ILLEGAL_COMMA,
                                               ' ');
                        ret = FAILURE;
                        continue;
                    }
                    break;
                    /* The IN_OPERAND state processes an operand part of the line: */
                case IN_OPERAND:
                    /** If the character is a comma, mark the end of the current word and transition to AFTER_OPERAND_AND_WAITING state.
                    *  If the character is not alphanumeric, or '@', '-', '+', transition to EXPECTING_COMMA state.
                    */
                    if (c == ',') {
                        if (!isspace(buffer[i - 1])) {
                            buffer[i] = '\0';
                        }
                        state = AFTER_OPERAND_AND_WAITING;
                    } else if (!(isalnum(c) || c == '-' || c == '+' ||
                                 c == '@')) {
                        state = EXPECTING_COMMA;
                    }
                    break;

                    /* The EXPECTING_COMMA state is entered when a comma is expected as a delimiter after an operand: */
                case EXPECTING_COMMA:
                    /** If the character is alphanumeric, handle it as an error since a comma was expected, mark the end of the previous word, start a new word, and transition back to IN_OPERAND state.
                    *  If the character is a comma, mark the end of the current word and transition to AFTER_OPERAND state.
                    *  If the character is a space, ignore it and continue waiting for a comma.
                    */
                    if (isalnum(c)) {
                        PrintCommaErrorMessage(line_number, MISSING_COMMA,
                                               c);
                        ret = FAILURE;
                        buffer[i - 1] = '\0';
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        buffer[i] = '\0';
                        state = AFTER_OPERAND;
                    } else if (isspace(c)) {
                        continue;  /* Ignore spaces while expecting a comma */
                    }
                    break;

                    /* The AFTER_OPERAND_AND_WAITING state is entered after an operand has been processed and we are waiting for another operand or the end of the line: */
                case AFTER_OPERAND_AND_WAITING:
                    /** If the character is alphanumeric, or '@', '-', '+', start a new word and transition back to IN_OPERAND state.
                    *  If the character is a comma, handle it as an error since consecutive commas are not allowed.
                    *  If the character is a space, ignore it and continue waiting for an alphanumeric character or a comma.
                    *  If the character is not alphanumeric, or '@', '-', '+', or a comma, transition to AFTER_OPERAND state.
                    */
                    if (isalnum(c) || c == '-' || c == '+' || c == '@') {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number,
                                               MULTIPLE_CONSECUTIVE_COMMAS,
                                               ' ');
                        ret = FAILURE;
                        continue;
                    } else if (isspace(c)) {
                        continue;
                    } else {
                        state = AFTER_OPERAND;
                    }
                    break;
                    /* The AFTER_OPERAND state is entered after an operand has been processed: */
                case AFTER_OPERAND:
                    /** If the character is alphanumeric, or '@', '-', '+', start a new word and transition back to IN_OPERAND state.
                    *  If the character is a comma, handle it as an error since consecutive commas are not allowed.
                    */
                    if (isalnum(c) || c == '-' || c == '+' || c == '@') {
                        input_words[(num_of_words)++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        PrintCommaErrorMessage(line_number,
                                               MULTIPLE_CONSECUTIVE_COMMAS,
                                               ' ');
                        ret = FAILURE;
                        continue;
                    }
                    break;
            }

        }
        /* Trim trailing spaces from the operand */
        for (i = num_of_words - 1; i >= 0; i--) {
            len = strlen(input_words[i]);
            while (len > 0 && isspace(input_words[i][len - 1])) {
                input_words[i][len - 1] = '\0';
                len--;
            }
        }
        /**
         * This block checks if the last non-space character is a comma, which is an error in the input format.
         * If a comma is found at the end of the line, it prints an error message and updates the return status to FAILURE.
         */
        if (last_non_space == ',') {
            PrintCommaErrorMessage(line_number, EXTRA_COMMA_END_OF_LINE,
                                   ' ');
            ret = FAILURE;
        }
        input_words[num_of_words] = NULL;

        /**
         * Updates the line numbers for the labels, checks for commands and instructions
         * in the input line, and updates the program state accordingly.
         */
        UpdateLines(input_words, num_of_words, has_label, programState);

        /* allocate memory for a new line */
        new_line = (Line *) malloc(sizeof(Line));
        if (new_line == NULL) {
            printf("Failed to allocate memory\n");
            fclose(file);
            fclose(bin_fp);
            return FAILURE;
        }

        /* allocate memory for the words and copy them */
        new_line->input_words = malloc(num_of_words * sizeof(char *));
        for (i = 0; i < num_of_words; ++i) {
            new_line->input_words[i] = my_strdup(input_words[i]);
        }

        new_line->line_number = line_number;
        new_line->num_of_words = num_of_words;
        new_line->has_label = has_label;

        /* Process each line and update the program state accordingly */
        ret += ProcessLine(new_line, bin_fp, programState);


        /* Free the allocated memory for input_words and the new line */
        for (i = 0; i < new_line->num_of_words; ++i) {
            free(new_line->input_words[i]);
        }
        free(new_line->input_words);  /*Free the input_words array*/

        free(new_line);
        ++line_number;
    }
    /* Closing file streams */
    fclose(file);
    fclose(bin_fp);

    return ret;
}
/******************************************************************************/