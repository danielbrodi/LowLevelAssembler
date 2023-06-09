#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 80

bool checkRegisterFormat(const char *line, int lineNumber) {
    const char *substring = strstr(line, "@r");
    while (substring != NULL) {
        int number;
        if (!(sscanf(substring + 2, "%d", &number) == 1 && number >= 1 && number <= 7)) 
        {
            printf("Error: after '@r' must be an integer between 1 and 7. Line: %d\n", lineNumber);
            return false;
        }
        substring = strstr(substring + 1, "@r");
    }
    return true;
}

int main() {
    FILE *file = fopen("myprog.am", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int lineNumber = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        lineNumber++;
        checkRegisterFormat(line, lineNumber);
    }

    fclose(file);

    return 0;
}

