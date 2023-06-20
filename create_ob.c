#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "macro.h"
#include "utils.h"
#include "binary.h"

/* Function to convert binary to decimal */
int binaryToDecimal(char *binary) {
    int decimal = 0;
    int len = strlen(binary);
    int i = 0;
    for (i = 0; i < len; i++) {
        if (binary[i] == '1')
            decimal += 1 << (len - i - 1);
    }

    return decimal;
}

/* Function to convert decimal to base64 character */
char decimalToBase64(int decimal) {
    if (decimal < 26)
        return 'A' + decimal;
    else if (decimal < 52)
        return 'a' + (decimal - 26);
    else if (decimal < 62)
        return '0' + (decimal - 52);
    else if (decimal == 62)
        return '+';
    else
        return '/';
}

void binaryToBase64(const char *input_file, const char *output_file) {
    int decimal1 = -1, decimal2 = -1;
    char base64_1 = -1, base64_2 = -1;
    char buffer[9];  /* Buffer to store binary digits (8 + '\0') */
    FILE *inputFile = NULL, *outputFile = NULL;

    inputFile = fopen(input_file, "r");
    if (NULL == inputFile) {
        printf("Failed to open the file.\n");
        exit(1);
    }
    outputFile = fopen(output_file, "w");
    if (NULL == outputFile) {
        printf("Failed to open the file.\n");
        exit(1);
    }

    /* Read the file line by line */
    while (fgets(buffer, sizeof(buffer), inputFile)) {
        /* Divide the binary number into two parts of length 6 */
        char part1[7];
        char part2[7];
        strncpy(part1, buffer, 6);
        part1[6] = '\0';
        strncpy(part2, buffer + 6, 6);
        part2[6] = '\0';

        /* Convert each part from binary to decimal */
        decimal1 = binaryToDecimal(part1);
        decimal2 = binaryToDecimal(part2);

        /* Convert each decimal to base64 character */
        base64_1 = decimalToBase64(decimal1);
        base64_2 = decimalToBase64(decimal2);

        /* Write the base64 characters to the output file */
        fputc(base64_1, outputFile);
        fputc(base64_2, outputFile);
        fputc('\n', outputFile);
    }

    /* Close the files */
    fclose(inputFile);
    fclose(outputFile);
}

