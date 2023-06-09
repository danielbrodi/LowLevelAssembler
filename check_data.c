#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80

bool is_valid_data_line(const char *line) {
    const char *data_ptr = strstr(line, ".data");
    if (data_ptr == NULL)
        return false;

    /* Move the pointer to the characters after ".data" */
    data_ptr += 5;

    /* Check if only numbers, spaces, tabs, "+" and "-" signs follow ".data" */
    while (*data_ptr != '\0') {
        if (!isdigit(*data_ptr) && !isspace(*data_ptr) && *data_ptr != '+' && *data_ptr != '-' && *data_ptr != ',')
        {
            printf("Invalid parameter of the .data instruction\n");
            return false;
        }
        data_ptr++;
    }

    /* Check if the last character is a comma (only for the line containing ".data") */
    int line_length = strlen(line);
    if (line[line_length - 1] == ',' && strstr(line, ".data") != NULL) 
    {
        printf("Invalid line - Last character is a comma\n");
        return false;
    }
   

    return true;
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

    /* Read the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        /* Remove the trailing newline character */
        line[strcspn(line, "\n")] = '\0';

        if (is_valid_data_line(line)) {
            printf("Valid line (%d): %s\n", line_number, line);
        } else if (strstr(line, ".data")) {
            printf("The .data instruction is invalid (%d): %s\n", line_number, line);
        }

        line_number++;
    }

    /* Close the file */
    fclose(file);

    return 0;
}

