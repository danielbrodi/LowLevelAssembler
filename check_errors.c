#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

#define MAX_LINE_LENGTH 80

Boolean isInteger(const char *str) {
    if (*str == '-' || *str == '+')
        str++; /* Skip sign if present*/

    while (*str != '\0') {
        if (!isdigit(*str))
            return FALSE;
        str++;
    }

    return TRUE;
}

Boolean hasNonIntegerNumbers(FILE *file) {
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, " \t\n");

        while (token != NULL) {
            size_t len = strlen(token);
            if (len > 0 && isdigit(token[0])) {
                if (token[len - 1] == ',')
                    token[len - 1] = '\0'; /* Remove comma*/

                if (!isInteger(token))
                    return TRUE;
            }

            token = strtok(NULL, " \t\n");
        }
    }

    return FALSE;
}

Boolean checkStringFormat(const char *line, int lineNumber) {
    const char *substring = strstr(line, ".string");
    if (substring != NULL) {
        const char *quoteStart = strchr(substring + 7, '\"');
        if (quoteStart != NULL) {
            const char *quoteEnd = strchr(quoteStart + 1, '\"');
            if (quoteEnd != NULL) {
                const char *nextChar = quoteEnd + 1;
                while (*nextChar != '\0' &&
                       (*nextChar == ' ' || *nextChar == '\t')) {
                    nextChar++;
                }
                if (*nextChar != '\0' && !isspace(*nextChar)) {
                    printf("Invalid string format found. Line: %d\n",
                           lineNumber);
                    return FALSE;
                }
                return TRUE;
            }
        }
        printf("Invalid string format found. Line: %d\n", lineNumber);
        return FALSE;
    }
    return TRUE;
}

Boolean checkRegisterFormat(const char *line, int lineNumber) {
    const char *substring = strstr(line, "@r");
    while (substring != NULL) {
        int number;
        if (!(sscanf(substring + 2, "%d", &number) == 1 && number >= 1 &&
              number <= 7)) {
            printf("Error: after '@r' must be an integer between 1 and 7. Line: %d\n",
                   lineNumber);
            return FALSE;
        }
        substring = strstr(substring + 1, "@r");
    }
    return TRUE;
}

Boolean is_valid_data_line(const char *line, int lineNumber) {
    const char *data_ptr = strstr(line, ".data");
    if (data_ptr == NULL)
        return FALSE;

    /* Move the pointer to the characters after ".data" */
    data_ptr += 5;

    /* Check if only numbers, spaces, tabs, "+" and "-" signs follow ".data" */
    while (*data_ptr != '\0') {
        if (!isdigit(*data_ptr) && !isspace(*data_ptr) && *data_ptr != '+' &&
            *data_ptr != '-' && *data_ptr != ',') {
            printf("Invalid parameter of the .data instruction. Line: %d\n",
                   lineNumber);
            return FALSE;
        }
        data_ptr++;
    }
    return TRUE;
}

int main() {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 1;

    /* Open the file */
    file = fopen("myprog.am", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    if (hasNonIntegerNumbers(file))
        printf("The file contains non-integer numbers.\n");
    rewind(file);

    /* Read the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        is_valid_data_line(line, line_number);
        checkRegisterFormat(line, line_number);
        checkStringFormat(line, line_number);
        line_number++;
    }


    /* Close the file */
    fclose(file);

    return 0;
}