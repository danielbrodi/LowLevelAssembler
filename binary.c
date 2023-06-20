#include <stdio.h>

void printBinary(int value, int numBits, FILE *outputFile) {
    int i;
    for (i = numBits - 1; i >= 0; i--) {
        int mask = 1 << i;
        int bit = (value & mask) ? 1 : 0;
        fprintf(outputFile, "%d", bit);
    }
}

void printBinaryCommand(int commandNum, int firstParamType, int secondParamType,
                        FILE *outputFile) {
    /* Print the last 3 bits for the first parameter type */
    printBinary(firstParamType, 3, outputFile);

    /* Print the next 4 bits for the command number */
    printBinary(commandNum, 4, outputFile);

    /* Print the next 3 bits for the second parameter type */
    printBinary(secondParamType, 3, outputFile);

    /* Print the first 2 bits as "00" */
    fprintf(outputFile, "00");

    fprintf(outputFile, "\n");
}

void printBinaryPrameterInteger(int number, FILE *outputFile) {
    /* Check if the number is negative*/
    if (number < 0) {
        /* Convert the negative number to its two's complement representation*/
        number = (1 << 10) + number;
    }
    /* Print the next 10 bits for the type of the integer*/
    printBinary(number, 10, outputFile);

    /* Print the first 2 bits as "00"*/
    fprintf(outputFile, "00");
    fprintf(outputFile, "\n");
}

void printBinaryPrameterRegister(int sourceOperand, int targetOperand,
                                 FILE *outputFile) {
    printf("First register id: %d, second register id: %d\n", sourceOperand,
           targetOperand);
    /*Print the next 5 bits for the source operand*/
    printBinary(sourceOperand, 5, outputFile);

    /*Print the next 5 bits for the target operand*/
    printBinary(targetOperand, 5, outputFile);

    /* Print the first 2 bits as "00"*/
    fprintf(outputFile, "00");

    fprintf(outputFile, "\n");
}

void printBinaryrPameterLabelEntry(int labelCode, FILE *outputFile) {

    /*Print the next 10 bits for the label code*/
    printBinary(labelCode, 10, outputFile);

    /*Print the first 2 bits as "10"*/
    fprintf(outputFile, "10");

    fprintf(outputFile, "\n");

}

void printBinaryrPameterLabelExtern(FILE *outputFile) {
    /*Print the first 10 bits as "0"*/
    fprintf(outputFile, "0000000000");

    /*Print the first 2 bits as "01"*/
    fprintf(outputFile, "01");

    fprintf(outputFile, "\n");
}

void printBinaryString(const char *str, FILE *outputFile) {
    int i = 0;
    int j = 0;
    while (str[i] != '\0') {
        if (str[i] != '\'') {
            printBinary((int) str[i], 12, outputFile);
            fprintf(outputFile, "\n");
        }
        i++;
    }

    /*Print 12 zeros*/
    for (j = 0; j < 12; j++) {
        fprintf(outputFile, "0");
    }
    fprintf(outputFile, "\n");
}

void printBinaryDataPrameter(int number, FILE *outputFile) {
    /* Check if the number is negative*/
    if (number < 0) {
        /* Convert the negative number to its two's complement representation*/
        number = (1 << 12) + number;
    }
    /* Print the next 10 bits for the type of the integer*/
    printBinary(number, 12, outputFile);
    fprintf(outputFile, "\n");
}


