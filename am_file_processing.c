#include "am_file_processing.h"

Status
ParseFile(char *am_file_name, char *bin_file_name, ProgramState *programState) {
    Status ret = SUCCESS;
    FILE *file = NULL;
    char buffer[80];
    char *input_words[80];
    Line *new_line = NULL;
    int line_number = 1;
    State state = -1;
    int num_of_words = 0;
    int i = 0, j = 0;
    int has_label = 0; /* A flag indicating if a label exists */
    size_t len = -1;
    char last_non_space = '\0';
    FILE *bin_fp = fopen(bin_file_name, "w");
    file = fopen(am_file_name, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return FAILURE;
    }

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
        state = IN_LABEL_OR_COMMAND;
        has_label = 0; /* A flag indicating if a label exists */
        input_words[(num_of_words)++] = buffer;

        for (i = 0; buffer[i] != '\0'; i++) {
            char c = buffer[i];
            if (!isspace(c)) {
                last_non_space = c;
            }
            switch (state) {
                case IN_LABEL_OR_COMMAND:
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
                case AFTER_LABEL_OR_COMMAND:
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
                case IN_COMMAND:
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
                case AFTER_COMMAND:
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
                case IN_OPERAND:
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


                case EXPECTING_COMMA:
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


                case AFTER_OPERAND_AND_WAITING:
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

                case AFTER_OPERAND:
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
        if (last_non_space == ',') {
            PrintCommaErrorMessage(line_number, EXTRA_COMMA_END_OF_LINE,
                                   ' ');
            ret = FAILURE;
        }
        input_words[num_of_words] = NULL;

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

        /* process each line */
        ret += ProcessLine(new_line, bin_fp, programState);


        /* Free the allocated memory for input_words */
        for (i = 0; i < new_line->num_of_words; ++i) {
            free(new_line->input_words[i]);
        }
        free(new_line->input_words);  /*Free the input_words array*/

        free(new_line);
        ++line_number;
    }
    fclose(file);
    fclose(bin_fp);

    return ret;
}
