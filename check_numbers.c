#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool isInteger(const char *str) {
    if (*str == '-' || *str == '+')
        str++; /* Skip sign if present*/

    while (*str != '\0') {
        if (!isdigit(*str))
            return false;
        str++;
    }

    return true;
}

bool hasNonIntegerNumbers(FILE *file) {
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, " \t\n");

        while (token != NULL) {
            size_t len = strlen(token);
            if (len > 0 && isdigit(token[0])) {
                if (token[len - 1] == ',')
                    token[len - 1] = '\0'; /* Remove comma*/

                if (!isInteger(token))
                    return true;
            }

            token = strtok(NULL, " \t\n");
        }
    }

    return false;
}

int main() {

    FILE *file = fopen("myprog.am", "r");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return 1;
    }

    if (hasNonIntegerNumbers(file))
        printf("The file contains non-integer numbers.\n");


    fclose(file);
    return 0;
}
