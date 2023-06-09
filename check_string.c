#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80

bool checkStringFormat(const char *line, int lineNumber) {
    const char *substring = strstr(line, ".string");
    if (substring != NULL) {
        const char *quoteStart = strchr(substring + 7, '\"');
        if (quoteStart != NULL) {
            const char *quoteEnd = strchr(quoteStart + 1, '\"');
            if (quoteEnd != NULL) {
                const char *nextChar = quoteEnd + 1;
                while (*nextChar != '\0' && (*nextChar == ' ' || *nextChar == '\t')) {
                    nextChar++;
                }
                if (*nextChar != '\0' && !isspace(*nextChar)) {
                    printf("Invalid string format found. Line: %d\n", lineNumber);
                    return false;
                }
                return true;
            }
        }
        printf("Invalid string format found. Line: %d\n", lineNumber);
        return false;
    }
    return true;
}

int main() {
    char line[MAX_LINE_LENGTH];
    int lineNumber = 0;
    FILE *file = fopen("myprog.am", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        lineNumber++;
        checkStringFormat(line, lineNumber);
    }

    fclose(file);

    return 0;
}

