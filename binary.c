#include <stdio.h>

void printBinary(int value, int numBits) {
    int i;
    for (i = numBits - 1; i >= 0; i--) {
        int mask = 1 << i;
        int bit = (value & mask) ? 1 : 0;
        printf("%d", bit);
    }
}

void
printBinaryCommand(int commandNum, int firstParamType, int secondParamType) {
    /* Print the last 3 bits for the first parameter type */
    printBinary(firstParamType, 3);

    /* Print the next 4 bits for the command number */
    printBinary(commandNum, 4);

    /* Print the next 3 bits for the second parameter type */
    printBinary(secondParamType, 3);

    /* Print the first 2 bits as "00" */
    printf("00");

    printf("\n");
}

void printBinaryPrameterInteger(int number) {
    /* Check if the number is negative*/
    if (number < 0) {
        /* Convert the negative number to its two's complement representation*/
        number = (1 << 10) + number;
    }
    /* Print the next 10 bits for the type of the integer*/
    printBinary(number, 10);

    /* Print the first 2 bits as "00"*/
    printf("00");
    printf("\n");
}

void printBinaryPrameterRegister(int sourceOperand, int targetOperand) {
    /*Print the next 5 bits for the source operand*/
    printBinary(sourceOperand, 5);

    /*Print the next 5 bits for the target operand*/
    printBinary(targetOperand, 5);

    /* Print the first 2 bits as "00"*/
    printf("00");

    printf("\n");
}

void printBinaryrPameterLabelEntry(int labelCode) {

    /*Print the next 10 bits for the label code*/
    printBinary(labelCode, 10);

    /*Print the first 2 bits as "10"*/
    printf("10");

    printf("\n");
}

void printBinaryrPameterLabelExtern() {
/*A label that is defined as external will have a code of -1*/
    /*Print the first 10 bits as "0"*/
    printf("0000000000");

    /*Print the first 2 bits as "01"*/
    printf("01");

    printf("\n");
}

void printBinaryString(const char *str) {
    int i = 0;
    int j = 0;
    while (str[i] != '\0') {
        if (str[i] != '\'') {
            printBinary((int) str[i], 12);
            printf("\n");
        }
        i++;
    }

    /*Print 12 zeros*/
    for (j = 0; j < 12; j++) {
        printf("0");
    }
    printf("\n");
}

void printBinaryDataPrameter(int number) {
    /* Check if the number is negative*/
    if (number < 0) {
        /* Convert the negative number to its two's complement representation*/
        number = (1 << 12) + number;
    }
    /* Print the next 10 bits for the type of the integer*/
    printBinary(number, 12);
}