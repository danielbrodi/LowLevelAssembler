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

void binaryToBase64(const char *input_file, const char *output_file, int IC, int DC) {
    FILE *inputFile = NULL, *outputFile = NULL;
    char buffer[13];  /* Buffer to store binary digits (12 + '\0') */
    char word1[7];
    char word2[7];

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
/*input_file*/
    /* Read the file line by line */
    fprintf(outputFile,"%d %d\n", IC, DC);
    while (fgets(buffer, sizeof(buffer), inputFile)) {
    
      if (buffer[0] == '\n')
            continue;
        strncpy(word1, buffer, 6);
        word1[6] = '\0';

        strncpy(word2, buffer + 6, 6);
        word2[6] = '\0';

        /* Convert each word to decimal */
        int decimal1 = binaryToDecimal(word1);
        int decimal2 = binaryToDecimal(word2);

        /* Convert each decimal to base64 character */
        char base641 = decimalToBase64(decimal1);
        char base642 = decimalToBase64(decimal2);

        /* Write the base64 characters to the output file */
        fputc(base641, outputFile);
        fputc(base642, outputFile);
        fputc('\n', outputFile);
         /* Print the binary words, decimal values, and base64 characters */
        printf("Binary: %s %s \t Decimal: %d %d \t Base64: %c %c\n", word1, word2, decimal1, decimal2, base641, base642);
   
    }
    
    /* Close the files */
    fclose(inputFile);
    fclose(outputFile);
}

