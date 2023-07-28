#include "binary_and_ob_processing.h"

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

void binaryToBase64(const char *input_file, const char *output_file, int IC,
                    int DC) {
    FILE *inputFile = NULL, *outputFile = NULL;
    char buffer[13];  /* Buffer to store binary digits (12 + '\0') */
    char word1[7];
    char word2[7];
    int decimal1 = -1, decimal2 = -1;
    char base64_1 = -1, base64_2 = -1;

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
    fprintf(outputFile, "%d %d\n", IC, DC);
    while (fgets(buffer, sizeof(buffer), inputFile)) {

        if (buffer[0] == '\n')
            continue;
        strncpy(word1, buffer, 6);
        word1[6] = '\0';

        strncpy(word2, buffer + 6, 6);
        word2[6] = '\0';

        /* Convert each word to decimal */
        decimal1 = binaryToDecimal(word1);
        decimal2 = binaryToDecimal(word2);

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

Status ProcessLine(Line *line, FILE *bin_fp, ProgramState *programState) {
    int i;
    char *command = line->input_words[line->has_label];
    int commandIdx = findCommand(command);
    int instructionIdx = findInstruction(command);
    int operandTypeIndex = 0;
    int paramIndex = 0;
    int paramTypes[2] = {0};
    int paramType = -1;
    int labelIdx = -1;
    char *paramWords[2] = {0};
    int expectedParamCount = -1;
    int first_register_id = -1;
    int second_register_id = -1;
    Label *label = NULL;
    ProgramState *currentProgramState = programState;

    if (commandIdx != -1) {
        currentProgramState->IC++;
        /* It's a command */
        /* Validate number of parameters */
        expectedParamCount = paramCount[commandIdx];
        if (line->num_of_words - 1 - line->has_label != expectedParamCount) {
            PrintCommandInstructionErrorMessage(line->line_number,
                                                INCORRECT_NUM_OF_PARAMS_FOR_COMMAND,
                                                command, NULL);
            return FAILURE;
        }
        /* Validate each parameter */
        for (i = 1 + line->has_label; i < line->num_of_words; i++) {
            OperandType expectedType = operandTypes[commandIdx][operandTypeIndex];
            while (expectedType == OPERAND_TYPE_NONE &&
                   operandTypeIndex < 2) {
                if (paramIndex >= 2) {
                    PrintCommandInstructionErrorMessage(line->line_number,
                                                        TOO_MANY_PARAMS_FOR_COMMAND,
                                                        command, NULL);
                    return FAILURE;
                }
                paramTypes[paramIndex++] = OPERAND_TYPE_NONE;
                expectedType = operandTypes[commandIdx][++operandTypeIndex];
            }
            if (expectedType == OPERAND_TYPE_NONE) {
                PrintCommandInstructionErrorMessage(line->line_number,
                                                    TOO_MANY_PARAMS_FOR_COMMAND,
                                                    command, NULL);
                return FAILURE;
            }
            if (!isValidParam(line->input_words[i], expectedType,
                              currentProgramState)) {
                PrintCommandInstructionErrorMessage(line->line_number,
                                                    INVALID_PARAM_FOR_COMMAND,
                                                    command,
                                                    line->input_words[i]);
                return FAILURE;
            }
            if (paramIndex >= 2) {
                PrintCommandInstructionErrorMessage(line->line_number,
                                                    TOO_MANY_PARAMS_FOR_COMMAND,
                                                    command, NULL);
                return FAILURE;
            }
            paramTypes[paramIndex] = findParameterType(line->input_words[i],
                                                       currentProgramState);
            paramWords[paramIndex] = line->input_words[i];
            paramIndex++;
            operandTypeIndex++;
        }


        printBinaryCommand(commandIdx, paramTypes[0], paramTypes[1], bin_fp);

        for (i = 0; i < 2; i++) {
            paramType = paramTypes[i];
            /*printf("The param %s is a %d!\n", paramWords[i], paramTypes[i]);*/
            switch (paramType) {
                case NUMBER:
                    currentProgramState->IC++;
                    printBinaryPrameterInteger(atoi(paramWords[i]), bin_fp);
                    break;
                case REGISTER:
                    /* When the command has an implicit first parameter (only one operand),
                       we treat the register as the second operand (target operand. */
                    if (paramCount[commandIdx] == 1 && i == 0) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &second_register_id);
                        currentProgramState->IC++;
                        /* If the current and next parameter are both registers, we parse both registers. */
                    } else if (i == 0 && paramTypes[i + 1] == REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d", &first_register_id);
                        sscanf(paramWords[i + 1], "%*[^0-9]%d",
                               &second_register_id);
                        currentProgramState->IC++;
                        /* If the previous parameter is not a register, we parse the current register as the second operand. */
                    } else if (i == 0 && paramTypes[i + 1] != REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &first_register_id);
                        currentProgramState->IC++;
                    } else if (i == 1 && paramTypes[i - 1] != REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &second_register_id);
                        currentProgramState->IC++;
                    }
                    break;

                case LABEL:
                    currentProgramState->IC++;
                    if (isLabel(paramWords[i], currentProgramState)) {
                        labelIdx = getLabelIndex(paramWords[i],
                                                 currentProgramState);
                        label = (Label *) programState->labels->items[labelIdx];
                        if (label->isExtern) {
                            printBinaryrPameterLabelExtern(bin_fp);
                        } else {
                            printBinaryrPameterLabelEntry(
                                    label->asm_line_number,
                                    bin_fp);
                        }
                    } else {
                        PrintLabelErrorMessage(line->line_number,
                                               LABEL_DOES_NOT_EXIST,
                                               paramWords[1]);
                    }
                    break;
            }
            if (first_register_id != -1 || second_register_id != -1) {
                /* If either first_register_id or second_register_id is not -1 (default),
                   we print the binary parameters. If one of them is -1 (default),
                   we substitute it with 0 for the function call. */
                printBinaryPrameterRegister(
                        first_register_id == -1 ? 0 : first_register_id,
                        second_register_id == -1 ? 0 : second_register_id,
                        bin_fp);
                /* We reset the first_register_id and second_register_id to -1
                   for the next iteration. */
                first_register_id = -1;
                second_register_id = -1;
            }
        }
    } else if (instructionIdx != -1) {
        /* It's an instruction */
        if (instructionIdx == ENTRY_INSTRUCTION ||
            instructionIdx == EXTERN_INSTRUCTION) {
            /* For entry and extern, there should only be one parameter */
            if (line->num_of_words - 1 - line->has_label != 1) {
                PrintCommandInstructionErrorMessage(line->line_number,
                                                    INCORRECT_NUM_OF_PARAMS_FOR_INSTRUCTION,
                                                    command, NULL);
                return FAILURE;
            }
            /* For entry, the label must exist */
            if (instructionIdx == ENTRY_INSTRUCTION) {
                if (!isLabelExists(line->input_words[1 + line->has_label],
                                   currentProgramState)) {
                    PrintLabelErrorMessage(line->line_number,
                                           ENTRY_REQUIRES_EXISTING_LABEL,
                                           line->input_words[1 +
                                                             line->has_label]);
                    return FAILURE;
                } else {
                    /* Mark the label as an entry */
                    label = (Label *) programState->labels->items[getLabelIndex(
                            line->input_words[line->has_label +
                                              1],
                            currentProgramState)];
                    label->isEntry = 1;
                }
            } else if (instructionIdx == EXTERN_INSTRUCTION) {
                if (isLabelExists(line->input_words[1 + line->has_label],
                                  currentProgramState)) {
                    PrintLabelErrorMessage(line->line_number,
                                           EXTERN_REQUIRES_NONEXISTING_LABEL,
                                           line->input_words[1 +
                                                             line->has_label]);
                    return FAILURE;
                } else {
                    /* Mark the label as an entry */
                    label = (Label *) programState->labels->items[getLabelIndex(
                            line->input_words[line->has_label +
                                              1],
                            currentProgramState)];
                    label->isExtern = 1;
                }
            }
        }
        switch (instructionIdx) {
            case STRING_INSTRUCTION:
                if (line->num_of_words - line->has_label != 2) {
                    PrintCommandInstructionErrorMessage(line->line_number,
                                                        INCORRECT_NUM_OF_PARAMS_FOR_INSTRUCTION,
                                                        command, NULL);
                    return FAILURE;
                }
                currentProgramState->DC +=
                        strlen(line->input_words[line->has_label + 1]) + 1;
                printBinaryString(line->input_words[line->has_label + 1],
                                  bin_fp);
                break;
            case DATA_INSTRUCTION:
                if (line->num_of_words - line->has_label < 2) {
                    PrintCommandInstructionErrorMessage(line->line_number,
                                                        INCORRECT_NUM_OF_PARAMS_FOR_INSTRUCTION,
                                                        command, NULL);
                    return FAILURE;
                }
                currentProgramState->DC +=
                        line->num_of_words - line->has_label - 1;
                for (i = line->has_label + 1; i < line->num_of_words; i++) {
                    printBinaryDataPrameter(atoi(line->input_words[i]), bin_fp);
                }
                break;
        }
    } else {
        PrintCommandInstructionErrorMessage(line->line_number,
                                            NOT_VALID_COMMAND_OR_INSTRUCTION,
                                            command, NULL);
    }
    return SUCCESS;
}
