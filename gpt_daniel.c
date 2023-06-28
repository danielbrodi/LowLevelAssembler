#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macro.h"
#include "utils.h"
#include "binary.h"
#include "create_ob.h"

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000
#define RUN_PROGRAM 1
#define STOP (-1)
#define ERROR (-1)

typedef enum {
    IN_LABEL_OR_COMMAND, AFTER_LABEL, IN_COMMAND, AFTER_COMMAND,
    AFTER_LABEL_OR_COMMAND, IN_OPERAND, AFTER_OPERAND, EXPECTING_COMMA,
    AFTER_OPERAND_AND_WAITING
} State;

typedef struct Line {
    char **input_words;
    int num_of_words;
    int has_label;
    struct Line *next;
} Line;

void PrintErrorMessage(int condition, int errorMessageId);

/** @param IC A pointer to the current instruction counter
 * @param DC A pointer to the current data counter*/

typedef enum {
    OPERAND_TYPE_NONE,
    OPERAND_TYPE_LABEL,
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_NUMBER,
    OPERAND_TYPE_LABEL_OR_REGISTER,
    OPERAND_TYPE_ALL
} OperandType;

typedef struct {
    char name[MAX_LABEL_LENGTH];
    int line_number;
    int asm_line_number;
    int isExtern;
    int isEntry;
} Label;


char *commandsList[] = {
        "mov",
        "cmp",
        "add",
        "sub",
        "not",
        "clr",
        "lea",
        "inc",
        "dec",
        "jmp",
        "bne",
        "red",
        "prn",
        "jsr",
        "rts",
        "stop"
};

char *instructionsList[] = {
        "data",
        "string",
        "entry",
        "extern"
};

char *registersList[] = {
        "r1",
        "r2",
        "r3",
        "r4",
        "r5",
        "r6",
        "r7",
};

int commandsListSize = sizeof(commandsList) / sizeof(char *);
int instructionsListSize = sizeof(instructionsList) / sizeof(char *);
int registersListSize = sizeof(registersList) / sizeof(char *);
Label labels[MAX_LINES];
Label externalLabels[MAX_LINES];
int label_count = 0;
int externalLabel_count = 0;
int current_line_number = 100;
int IC = 0, DC = 0;

OperandType operandTypes[][2] = {
        {OPERAND_TYPE_ALL,   OPERAND_TYPE_LABEL_OR_REGISTER}, /* mov */
        {OPERAND_TYPE_ALL,   OPERAND_TYPE_ALL},               /* cmp */
        {OPERAND_TYPE_ALL,   OPERAND_TYPE_LABEL_OR_REGISTER}, /* add */
        {OPERAND_TYPE_ALL,   OPERAND_TYPE_LABEL_OR_REGISTER}, /* sub */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* not */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* clr */
        {OPERAND_TYPE_LABEL, OPERAND_TYPE_LABEL_OR_REGISTER}, /* lea */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* inc */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* dec */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* jmp */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* bne */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* red */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_ALL},  /* prn */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_LABEL_OR_REGISTER}, /* jsr */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_NONE}, /* rts */
        {OPERAND_TYPE_NONE,  OPERAND_TYPE_NONE}  /* stop */
};


int paramCount[] = {
        2,  /* mov */
        2,  /* cmp */
        2,  /* add */
        2,  /* sub */
        1,  /* not */
        1,  /* clr */
        2,  /* lea */
        1,  /* inc */
        1,  /* dec */
        1,  /* jmp */
        1,  /* bne */
        1,  /* red */
        1,  /* prn */
        1,  /* jsr */
        0,  /* rts */
        0   /* stop */
};

typedef enum {
    DATA_INSTRUCTION,
    STRING_INSTRUCTION,
    ENTRY_INSTRUCTION,
    EXTERN_INSTRUCTION
} Instruction;

#define ILLEGAL_COMMA 0
#define MISSING_COMMA 1
#define EXTRA_COMMAS 2
#define EXTRA_TEXT 3

typedef enum {
    OPERATION_NOT_EXIST = 0,
    TOO_MANY_OPERANDS,
    NOT_ENOUGH_OPERANDS,
    MULTIPLE_CONSECUTIVE_COMMAS,
    REGISTER_NAME_NOT_EXIST,
    OPERAND_TYPE_MISMATCH,
    LABEL_ALREADY_DEFINED,
    LINE_TOO_LONG,
    DATA_INSTRUCTION_INVALID
} ErrorType;


const char *ErrorMessages[] = {
        "Operation doesn't exist.",
        "Too many operands provided.",
        "Not enough operands provided.",
        "Multiple consecutive commas.",
        "Register name does not exist.",
        "The operand type that does not match the operation.",
        "Multiple consecutive commas.",
        "Label already defined.",
        "The line is too long, maximum size of an input line is 80 charachters",
        ".data instruction must have at least one integer."
};

typedef enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
} Registers;

typedef enum {
    NUMBER = 1,
    LABEL = 3,
    REGISTER = 5
} Types;

typedef struct line {
    long number;
    char *file_name;
    char *content;
} line;

void to_lowercase(char *str) {
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int countParams(int num_of_words) {
    int count = num_of_words - 1;
    return count;
}

int startsWith(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

Status checkLabels(char *am_file_name) {
    int i = 0;
    char line[MAX_LINE_LENGTH] = {0};
    int line_number = 0;
    char new_label[MAX_LABEL_LENGTH] = {0};
    char *label_end = NULL;
    int label_length = -1;

    FILE *file = fopen(am_file_name, "r");

    if (!file) {
        printf("File not found: %s\n", am_file_name);
        return FAILURE;
    }

    /* First scan to validate and collect labels */
    while (fgets(line, sizeof(line), file)) {
        line_number++;

        /* Check if line starts with a label */
        label_end = strchr(line, ':');
        if (label_end) {
            /* Extract label */
            label_length = (int) (label_end - line);
            /* Check if label length is less than 31 */
            if (label_length >= MAX_LABEL_LENGTH) {
                printf("Error on line %d: Label length exceeds limit\n",
                       line_number);
                return FAILURE;
            }

            /* Check if label is a saved word */
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';
            to_lowercase(new_label);

            for (i = 0; i < commandsListSize; i++) {
                if (strcmp(new_label, commandsList[i]) == 0) {
                    printf("Error on line %d: Label is a reserved command word\n",
                           line_number);
                    return FAILURE;
                }
            }

            for (i = 0; i < instructionsListSize; i++) {
                if (strcmp(new_label, instructionsList[i]) == 0) {
                    printf("Error on line %d: Label is a reserved instruction word at line %d\n",
                           line_number);
                    return FAILURE;
                }
            }

            for (i = 0; i < registersListSize; i++) {
                if (strcmp(new_label, registersList[i]) == 0) {
                    printf("Label is a reserved register word at line %d\n",
                           line_number);
                    return FAILURE;
                }
            }

            /* Check if label contains spaces */
            for (i = 0; i < label_length; i++) {
                if (isspace(line[i])) {
                    printf("Invalid label format at line %d\n", line_number);
                    return FAILURE;
                }
            }

            /* Check if the first character is a letter */
            if (!isalpha(new_label[0])) {
                printf("Invalid label at line %d: Label must start with a letter!\n",
                       line_number);
                return FAILURE;
            }

            /* Check for duplicate labels */
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';
            for (i = 0; i < label_count; i++) {
                if (strcmp(new_label, labels[i].name) == 0) {
                    printf("Duplicate label %s at line %d\n", new_label,
                           line_number);
                    return FAILURE;
                }
            }

            /* Copy label to labels array */
            strncpy(labels[label_count].name, new_label, label_length + 1);
            labels[label_count].line_number = line_number;
            labels[label_count].isEntry = 0;
            labels[label_count].isExtern = 0;
            label_count++;
        } else {
            /* Check if line starts with an 'extern' instruction */
            if (startsWith(line, ".extern")) {
                /* Extract label */
                char *label_start = line + strlen(".extern") +
                                    1; /* +1 to skip the space after '.extern' */
                label_end = strpbrk(label_start,
                                    " \t\n"); /* find the end of the label */
                if (!label_end)
                    label_end = label_start +
                                strlen(label_start); /* end of string if no whitespace found */

                /* Check and add the label as before, but mark it as extern */
                label_length = (int) (label_end - label_start);
                strncpy(new_label, label_start, label_length);
                new_label[label_length] = '\0';

                /* Check for duplicate labels */
                for (i = 0; i < label_count; i++) {
                    if (strcmp(new_label, labels[i].name) == 0) {
                        printf("Duplicate label %s at line %d\n", new_label,
                               line_number);
                        return FAILURE;
                    }
                }

                /* Copy label to labels array */
                strncpy(labels[label_count].name, new_label, label_length + 1);
                labels[label_count].line_number = -1; /* No line number for extern labels */
                labels[label_count].isEntry = 0;
                labels[label_count].isExtern = 1;
                label_count++;
            }
        }
    }

    fclose(file);

    for (i = 0; i < label_count; i++) {
        printf("this is label %d: %s in line: %d\n", i, labels[i].name,
               labels[i].line_number);
    }

    return SUCCESS;
}

int isNumber(const char *str) {
    if (*str == '-' || *str == '+') /* Check if the number is negative */
        str++; /* Skip the minus sign */
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int getRegisterId(const char *str) {
    int i;
    for (i = 0; i < registersListSize; i++) {
        if (strcmp(str, registersList[i]) ==
            0) {
            return 1;
        }
    }
    return 0;
}

int isRegister(const char *str) {
    const char *operand = NULL;
    int i;
    if (str != NULL) {
        if (str[0] == '@') {
            operand = str + 1;
            for (i = 0; i < registersListSize; i++) {
                if (strcmp(operand, registersList[i]) == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int isLabel(const char *str) {
    int i;
    for (i = 0; i < label_count; i++) {
        if (strcmp(str, labels[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

int getLabelIndex(const char *str) {
    int i;
    for (i = 0; i < label_count; i++) {
        if (strcmp(str, labels[i].name) == 0) {
            return i;
        }
    }
    return 0;
}


Status preProcess(const char *input_file, const char *output_file) {
    int i;
    char line[1024];
    FILE *inputFile = NULL, *outputFile = NULL;
    Macro *macroToExpand = NULL;
    Macro *currentMacro = NULL;
    MacroVector *macroVector = new_macro_vector();

    inputFile = fopen(input_file, "r");
    if (NULL == inputFile) {
        return FAILURE;
    }
    outputFile = fopen(output_file, "w");
    if (NULL == outputFile) {
        if (inputFile != NULL) {
            fclose(inputFile);
        }
        return FAILURE;
    }

    /* First pass: build the list of macros */
    while (fgets(line, sizeof(line), inputFile)) {
        if (1 == filter_line(line)) {
            continue;
        }
        /* If the line starts a macro definition */
        if (strncmp(line, "mcro", 4) == 0) {
            /* Remove the newline at the end of the line, if present */
            if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
            /* Start a new macro definition */
            currentMacro = new_macro(line + 5);
            push_back_macro(macroVector, currentMacro);
        } else if (strncmp(line, "endmcro", 7) == 0) {
            /* End of the current macro definition */
            currentMacro = NULL;
        } else if (currentMacro) {
            /* Inside a macro definition: add the line to the macro's command list */
            char *command = my_strdup(line);
            size_t length = strlen(command);
            if (length > 0 && command[length - 1] == '\n') {
                command[length - 1] = '\0';
            }
            push_back(currentMacro->commands, command);
            free(command);
        }
    }

/* Rewind the input file to the beginning for the second pass */
    rewind(inputFile);

/* Second pass: output the file, expanding macros */
    while (fgets(line, sizeof(line), inputFile)) {
        if (1 == filter_line(line)) {
            continue;
        }
        /* Remove the newline at the end of the line, if present */
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        /* If the line starts with a macro definition or end of macro, skip it */
        if (strncmp(line, "mcro", 4) == 0) {
            while (0 != strncmp(line, "endmcro", 7)) {
                fgets(line, sizeof(line), inputFile);
            }
            continue;
        }

        macroToExpand = NULL;
        for (i = 0; i < macroVector->size; ++i) {
            /* If the line starts with a macro invocation, expand it */
            if (strcmp(line, macroVector->macros[i]->name) == 0) {
                macroToExpand = macroVector->macros[i];
                break;
            }
        }

        if (macroToExpand) {
            /* Expand the macro */
            for (i = 0; i < macroToExpand->commands->size; ++i) {
                fputs(macroToExpand->commands->commands[i], outputFile);
                fputs("\n", outputFile);
            }
        } else {
            /* Otherwise, write the line to the output as is */
            fputs(line, outputFile);
            fputs("\n", outputFile);
        }
    }


    fclose(inputFile);
    fclose(outputFile);
    for (i = 0; i < macroVector->size; ++i) {
        free_macro(macroVector->macros[i]);
    }
    free(macroVector->macros);
    free(macroVector);

    return SUCCESS;
}

Boolean isLabelExists(char *label) {
    if (isLabel(label)) {
        if (!labels[getLabelIndex(label)].isExtern) {
            return TRUE;
        }
    }
    return FALSE;
}


int findInstruction(const char *instruction) {
    int instructionIdx;
    const char *comparisonInstruction = instruction;

    /* If the string starts with a dot, ignore the dot for comparison purposes */
    if (instruction[0] == '.') {
        comparisonInstruction = instruction + 1;
    }

    for (instructionIdx = 0;
         instructionIdx < instructionsListSize; instructionIdx++) {
        /* printf("Debug: Comparing '%s' and '%s'\n", comparisonInstruction,
               instructionsList[instructionIdx]); */
        if (strcmp(comparisonInstruction, instructionsList[instructionIdx]) ==
            0) {
            /* printf("Debug: Found instruction '%s'\n", comparisonInstruction); */
            return instructionIdx;
        }
    }
    /* printf("Debug: Instruction '%s' not found\n", comparisonInstruction); */
    return -1;
}


int findCommand(char *command) {
    int i;
    /* Convert command to lowercase */
    to_lowercase(command);
    for (i = 0; i < commandsListSize; i++) {
        if (strcmp(commandsList[i], command) == 0) {
            return i;
        }
    }
    return -1;
}

int isValidParam(char *param, OperandType expectedType) {
    switch (expectedType) {
        case OPERAND_TYPE_NONE:
            return 0;
        case OPERAND_TYPE_LABEL:
            return isLabel(param);
        case OPERAND_TYPE_REGISTER:
            return isRegister(param);
        case OPERAND_TYPE_NUMBER:
            return isNumber(param);
        case OPERAND_TYPE_LABEL_OR_REGISTER:
            return isLabel(param) || isRegister(param);
        case OPERAND_TYPE_ALL:
            return isLabel(param) || isRegister(param) || isNumber(param);
        default:
            return 0;
    }
}

int findParameterType(char *operand) {
    if (isNumber(operand)) {
        /*
        printf("Checking if %s is a number and the answer is %d.\n", operand,
               isNumber(operand));
               */
        return NUMBER;
    } else if (isLabel(operand)) {
        return LABEL;
    } else if (isRegister(operand)) {
        return REGISTER;
    } else {
        return OPERAND_TYPE_NONE; /* not a valid type */
    }
}

void addExternalLabel(int labelIdx, int lineNumber) {

    /* Add the label index to the next position in the externalLabels array */
    externalLabels[externalLabel_count] = labels[labelIdx];
    /* Add the line number to the lineNumber field */
    externalLabels[externalLabel_count].asm_line_number = lineNumber;
    /* Increment the counter for the next call */
    externalLabel_count++;
}


void UpdateLines(char *words[], int num_of_words, int has_label) {

    int i, commandIdx, commandOrderInWords, operandIdx;
    char *command;
    int labelIdx;

    if (has_label) {
        commandOrderInWords = 1;
    } else {
        commandOrderInWords = 0;
    }

    command = words[commandOrderInWords];
    commandIdx = findCommand(command);

    if (has_label) {
        for (i = 0; i < label_count; i++) {
            if (strcmp(words[0], labels[i].name) == 0) {
                labels[i].asm_line_number = current_line_number;
                break;
            }
        }
    }

    if (commandIdx != -1) { /* If it's a command and not an instruction */
        for (operandIdx = commandOrderInWords + 1; operandIdx <=
                                                   commandOrderInWords +
                                                   paramCount[commandIdx]; operandIdx++) {
            if (isLabel(words[operandIdx])) {
                labelIdx = getLabelIndex(words[operandIdx]);
                if (labels[labelIdx].isExtern) {
                    if (has_label) {
                        if (operandIdx == 2) {
                            addExternalLabel(labelIdx, current_line_number + 1);
                        } else {
                            addExternalLabel(labelIdx, current_line_number + 2);
                        }
                    } else {
                        if (operandIdx == 1) {
                            addExternalLabel(labelIdx, current_line_number + 1);
                        } else {
                            addExternalLabel(labelIdx, current_line_number + 2);
                        }
                    }
                }
            }
        }
        current_line_number += paramCount[commandIdx] + 1;
        /* If both parameters are registers, we decrement the line count by 1 */
        if (paramCount[commandIdx] == 2 &&
            isValidParam(words[commandOrderInWords + 1],
                         OPERAND_TYPE_REGISTER) &&
            isValidParam(words[commandOrderInWords + 2],
                         OPERAND_TYPE_REGISTER)) {
            current_line_number--;
        }
    } else { /* It's an instruction */
        if (strcmp(command, "string") == 0) {
            /* Including the null character at the end of the string */
            current_line_number += strlen(words[commandOrderInWords + 1]) + 1;
        } else if (strcmp(command, "data") == 0) {
            if (has_label) {
                current_line_number += num_of_words - 2;
            } else {
                current_line_number += num_of_words - 1;
            }
        }
    }
}

Status
ProcessLine(Line *line, FILE *bin_fp) {
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

    if (commandIdx != -1) {
        /* It's a command */
        /* Validate number of parameters */
        expectedParamCount = paramCount[commandIdx];
        if (line->num_of_words - 1 - line->has_label != expectedParamCount) {
            printf("Incorrect number of parameters for command '%s'\n",
                   command);
            return FAILURE;
        }
        /* Validate each parameter */
        for (i = 1 + line->has_label; i < line->num_of_words; i++) {
            OperandType expectedType = operandTypes[commandIdx][operandTypeIndex];
            while (expectedType == OPERAND_TYPE_NONE &&
                   operandTypeIndex < 2) {
                if (paramIndex >= 2) {
                    printf("Too many parameters for command '%s'\n",
                           command);
                    return FAILURE;
                }
                paramTypes[paramIndex++] = OPERAND_TYPE_NONE;
                expectedType = operandTypes[commandIdx][++operandTypeIndex];
            }
            if (expectedType == OPERAND_TYPE_NONE) {
                printf("Too many parameters for command '%s'\n",
                       command);
                return FAILURE;
            }
            if (!isValidParam(line->input_words[i], expectedType)) {
                printf("Invalid parameter '%s' for command '%s'\n",
                       line->input_words[i], command);
                return FAILURE;
            }
            if (paramIndex >= 2) {
                printf("Too many parameters for command '%s'\n",
                       command);
                return FAILURE;
            }
            paramTypes[paramIndex] = findParameterType(line->input_words[i]);
            paramWords[paramIndex] = line->input_words[i];
            paramIndex++;
            operandTypeIndex++;
            IC++;
        }


        printBinaryCommand(commandIdx, paramTypes[0], paramTypes[1], bin_fp);

        for (i = 0; i < 2; i++) {
            paramType = paramTypes[i];
            printf("The param %s is a %d!\n", paramWords[i], paramTypes[i]);
            switch (paramType) {
                case NUMBER:
                    IC++;
                    printBinaryPrameterInteger(atoi(paramWords[i]), bin_fp);
                    break;
                case REGISTER:
                    /* When the command has an implicit first parameter (only one operand),
                       we treat the register as the second operand (target operand. */
                    if (paramCount[commandIdx] == 1 && i == 0) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &second_register_id);
                        IC++;
                        /* If the current and next parameter are both registers, we parse both registers. */
                    } else if (paramTypes[i + 1] == REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d", &first_register_id);
                        sscanf(paramWords[i + 1], "%*[^0-9]%d",
                               &second_register_id);
                        IC++;
                        /* If the previous parameter is not a register, we parse the current register as the second operand. */
                    } else if (i == 0 && paramTypes[i + 1] != REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &first_register_id);
                        IC++;
                    } else if (i == 1 && paramTypes[i - 1] != REGISTER) {
                        sscanf(paramWords[i], "%*[^0-9]%d",
                               &second_register_id);
                        IC++;
                    }
                    break;

                case LABEL:
                    IC++;
                    if (isLabel(paramWords[i])) {
                        labelIdx = getLabelIndex(paramWords[i]);
                        if (labels[labelIdx].isExtern) {
                            printBinaryrPameterLabelExtern(bin_fp);
                        } else {
                            printBinaryrPameterLabelEntry(
                                    labels[labelIdx].asm_line_number, bin_fp);
                        }
                    } else {
                        printf("Error! label %s does not exist neither as a label nor as an extern label.\n",
                               paramWords[i]);
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
        /* printf("Debug: Processing instruction '%s'\n", words[has_label]); */
        if (instructionIdx == ENTRY_INSTRUCTION ||
            instructionIdx == EXTERN_INSTRUCTION) {
            /* For entry and extern, there should only be one parameter */
            /* printf("Debug: num_of_words = %d, has_label = %d, label = '%s'\n",
                   num_of_words, has_label, words[1 + has_label]);*/
            if (line->num_of_words - 1 - line->has_label != 1) {
                printf("Incorrect number of parameters for instruction '%s'\n",
                       command);
                return FAILURE;
            }
            /* For entry, the label must exist */
            if (instructionIdx == ENTRY_INSTRUCTION) {
                if (!isLabelExists(line->input_words[1 + line->has_label])) {
                    printf("Error: entry requires label that exists but Label '%s' does not exist\n",
                           line->input_words[1 + line->has_label]);
                    return FAILURE;
                } else {
                    /* Mark the label as an entry */
                    labels[getLabelIndex(line->input_words[line->has_label + 1])].isEntry = 1;
                }
            }
                /* For extern, the label must not exist */
            else if (instructionIdx == EXTERN_INSTRUCTION) {
                if (isLabelExists(line->input_words[1 + line->has_label])) {
                    printf("Error: extern requires label that doesn't exist but Label '%s' already exists\n",
                           line->input_words[1 + line->has_label]);
                    return FAILURE;
                } else {
                    /* Mark the label as an entry */
                    labels[getLabelIndex(line->input_words[line->has_label + 1])].isExtern = 1;
                }
            }
        }
        switch (instructionIdx) {
            case STRING_INSTRUCTION:
                DC += strlen(line->input_words[line->has_label + 1]) + 1;
                printBinaryString(line->input_words[line->has_label + 1], bin_fp);
                break;
            case DATA_INSTRUCTION:
                DC += line->num_of_words - line->has_label - 1;
                for (i = line->has_label + 1; i < line->num_of_words; i++) {
                    printBinaryDataPrameter(atoi(line->input_words[i]), bin_fp);
                }
                break;
        }
    } else {
        printf("Error: \'%s\' is not a valid command or instruction!\n",
               command);
    }

    /* If there's a label, validate it */
    if (line->has_label) {
        if (!isLabel(line->input_words[0])) {
            printf("Invalid label '%s'\n", line->input_words[0]);
            return FAILURE;
        }
    }
    return SUCCESS;
}

void WriteLabelsToFile(const char *ent_filename, const char *ext_filename) {
    int i = 0;
    FILE *entry_fp = NULL;
    FILE *extern_fp = NULL;

    for (i = 0; i < externalLabel_count; i++) {
        if (extern_fp == NULL) {
            extern_fp = fopen(ext_filename, "w");
            if (extern_fp == NULL) {
                printf("Unable to open external file (%s) for writing.\n",
                       ext_filename);
                return;
            }
        }
        fprintf(extern_fp, "%s %d\n", externalLabels[i].name,
                externalLabels[i].asm_line_number);
    }

    for (i = 0; i < label_count; i++) {
        if (labels[i].isEntry) {
            if (entry_fp == NULL) {
                entry_fp = fopen(ent_filename, "w");
                if (entry_fp == NULL) {
                    printf("Unable to open entry file (%s) for writing.\n",
                           ent_filename);
                    /* Close already opened extern file */
                    if (extern_fp != NULL) {
                        fclose(extern_fp);
                    }
                    return;
                }
            }
            fprintf(entry_fp, "%s %d\n", labels[i].name,
                    labels[i].asm_line_number);
        }
    }

    if (entry_fp != NULL) {
        fclose(entry_fp);
    }
    if (extern_fp != NULL) {
        fclose(extern_fp);
    }
}


Status ParseFile(char *am_file_name, char *bin_file_name) {
    Status ret = SUCCESS;
    FILE *file = NULL;
    char buffer[80];
    char *input_words[80];
    Line *first_line = NULL, *current_line = NULL;
    int line_number = 1;
    State state = -1;
    int num_of_words = 0;
    int i = 0, j = 0;
    int has_label = 0; /* A flag indicating if a label exists */
    size_t len = -1;
    char last_non_space = '\0';
    FILE *bin_fp = fopen(bin_file_name, "w");
    file = fopen(am_file_name, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return FAILURE;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        /* Remove newline character at the end of the buffer, if exists */
        len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        /* Skip quotes */
        for (i = 0, j = 0; i < len; i++) {
            if (buffer[i] != '"') {
                buffer[j++] = buffer[i];
            }
        }
        buffer[j] = '\0';
        num_of_words = 0;
        state = IN_LABEL_OR_COMMAND;
        has_label = 0; /* A flag indicating if a label exists */
        input_words[(num_of_words)++] = buffer;

        for (i = 0; buffer[i] != '\0'; i++) {
            char c = buffer[i];
            if (!isspace(c)) {
                last_non_space = c;
            }
            switch (state) {
                case IN_LABEL_OR_COMMAND:
                    if (isspace(c)) {
                        buffer[i] = '\0';
                        state = AFTER_LABEL_OR_COMMAND;
                    } else if (c == ',') {
                        PrintErrorMessage(1, ILLEGAL_COMMA);
                        ret = FAILURE;
                        continue;
                    } else if (c == ':') {
                        has_label = 1;  /* Mark that a label is present */
                        buffer[i] = '\0';
                        state = AFTER_LABEL;
                    }
                    break;
                case AFTER_LABEL:
                    if (isalnum(c)) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_COMMAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Illegal comma\n",
                               line_number);
                        ret = FAILURE;
                        continue;  /* Continue with next character */
                    }
                    break;
                case AFTER_LABEL_OR_COMMAND:
                    if (has_label && isalnum(c)) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_COMMAND;
                    } else if (!has_label &&
                               (isalnum(c) || c == '@' || c == '-' ||
                                c == '+')) {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Illegal comma\n",
                               line_number);
                        ret = FAILURE;
                        continue;  /* Continue with next character */
                    }
                    break;
                case IN_COMMAND:
                    if (isspace(c)) {
                        buffer[i] = '\0';
                        state = AFTER_COMMAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Illegal comma\n",
                               line_number);
                        ret = FAILURE;
                        continue;
                    }
                    break;
                case AFTER_COMMAND:
                    if (isalnum(c) || c == '-' || c == '+' || c == '@') {
                        input_words[(num_of_words)++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Illegal comma\n",
                               line_number);
                        ret = FAILURE;
                        continue;
                    }
                    break;
                case IN_OPERAND:
                    if (c == ',') {
                        if (!isspace(buffer[i - 1])) {
                            buffer[i] = '\0';
                        }
                        state = AFTER_OPERAND_AND_WAITING;
                    } else if (!(isalnum(c) || c == '-' || c == '+' ||
                                 c == '@')) {
                        state = EXPECTING_COMMA;
                    }
                    break;


                case EXPECTING_COMMA:
                    if (isalnum(c)) {
                        printf("Error on line %d: Missing comma before char %c\n",
                               line_number, c);
                        ret = FAILURE;
                        buffer[i - 1] = '\0';
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        buffer[i] = '\0';
                        state = AFTER_OPERAND;
                    } else if (isspace(c)) {
                        continue;  /* Ignore spaces while expecting a comma */
                    }
                    break;


                case AFTER_OPERAND_AND_WAITING:
                    if (isalnum(c) || c == '-' || c == '+' || c == '@') {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Multiple consecutive commas\n",
                               line_number);
                        ret = FAILURE;
                        continue;
                    } else if (isspace(c)) {
                        continue;
                    } else {
                        state = AFTER_OPERAND;
                    }
                    break;

                case AFTER_OPERAND:
                    if (isalnum(c) || c == '-' || c == '+') {
                        input_words[(num_of_words)++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Multiple consecutive commas\n",
                               line_number);
                        ret = FAILURE;
                        continue;
                    }
                    break;
            }

        }
        /* Trim trailing spaces from the operand */
        for (i = num_of_words - 1; i >= 0; i--) {
            len = strlen(input_words[i]);
            while (len > 0 && isspace(input_words[i][len - 1])) {
                input_words[i][len - 1] = '\0';
                len--;
            }
        }
        if (last_non_space == ',') {
            printf("Error on line %d: Extra comma at the end of the line\n",
                   line_number);
            ret = FAILURE;
        }
        input_words[num_of_words] = NULL;
        /*for (i = 0; i < num_of_words; i++) {
            printf("For line %d the word idx %d is: \"%s\".\n",
                   line_number,
                   i,
                   input_words[i]);
        }*/
        ++line_number;
        UpdateLines(input_words, num_of_words, has_label);

        /* allocate memory for a new line */
        Line *new_line = (Line *) malloc(sizeof(Line));
        if (new_line == NULL) {
            printf("Failed to allocate memory\n");
            fclose(file);
            return FAILURE;
        }

        /* allocate memory for the words and copy them */
        new_line->input_words = malloc(num_of_words * sizeof(char *));
        for (i = 0; i < num_of_words; ++i) {
            new_line->input_words[i] = my_strdup(input_words[i]);
        }

        new_line->num_of_words = num_of_words;
        new_line->has_label = has_label;
        new_line->next = NULL;

        /* If it's the first node, it's the head */
        if (first_line == NULL) {
            first_line = new_line;
            current_line = first_line;
        } else {
            current_line->next = new_line;
            current_line = new_line;
        }
    }
    fclose(file);

    /* process each line */
    Line *line = first_line;
    while(line != NULL) {
        ret += ProcessLine(line, bin_fp);
        line = line->next;
    }
    fclose(bin_fp);

   /* free memory */
    line = first_line;
    while(line != NULL) {
        Line *tmp = line;
        line = line->next;
        free(tmp);
    }
    return ret;
}

int main(int argc, char *argv[]) {

    Status stages_status = SUCCESS;
    int i = 0, j = 0;
    FILE *file = NULL;
    if (argc < 2) {
        printf("Please provide file names as command-line arguments.\n");
        return FAILURE;
    }
    for (i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
        /* Append extensions to the current file name */
        size_t argLength = strlen(argv[i]);
        char *file_name_as = malloc(argLength + 4); /* 4 for ".as\0" */
        char *file_name_am = malloc(argLength + 4); /* 4 for ".am\0" */
        char *file_name_ent = malloc(argLength + 5); /* 5 for ".ent\0" */
        char *file_name_ext = malloc(argLength + 5); /* 5 for ".ext\0" */
        char *file_name_bin = malloc(argLength + 5); /* 5 for ".bin\0" */
        char *file_name_ob = malloc(argLength + 4); /* 4 for ".ob\0" */

        sprintf(file_name_as, "%s.as", argv[i]);
        sprintf(file_name_am, "%s.am", argv[i]);
        sprintf(file_name_ent, "%s.ent", argv[i]);
        sprintf(file_name_ext, "%s.ext", argv[i]);
        sprintf(file_name_bin, "%s.bin", argv[i]);
        sprintf(file_name_ob, "%s.ob", argv[i]);


        file = fopen(file_name_as, "r");
        if (file == NULL) {
            printf("Failed to open the file: %s\n", file_name_as);
            free(file_name_as);
            free(file_name_am);
            free(file_name_ent);
            free(file_name_ext);
            free(file_name_bin);
            free(file_name_ob);
            continue;  /* skip to the next file */
        }

        stages_status += preProcess(file_name_as, file_name_am);
        stages_status += checkLabels(file_name_am);
        stages_status += ParseFile(file_name_am, file_name_bin);
        printf("THE STAGE STATUS IS: %d!\n", stages_status);
        if (SUCCESS == stages_status) {
            WriteLabelsToFile(file_name_ent, file_name_ext);
            binaryToBase64(file_name_bin, file_name_ob, IC, DC);
        }
        for (j = 0; j < label_count; j++) {
            printf("Label name: %s, line_number: %d, asm_line_number: %d.\n",
                   labels[j].name, labels[j].line_number,
                   labels[j].asm_line_number);

            labels[j].name[0] = '\0';
            labels[j].line_number = 0;
            labels[j].asm_line_number = 0;
            labels[j].isExtern = 0;
            labels[j].isEntry = 0;

        }
        IC = 0, DC = 0;
        label_count = 0;
        externalLabel_count = 0;
        current_line_number = 100;

        /*remove(file_name_bin);*/

        fclose(file);  /* Close the file when done */
        free(file_name_as);
        free(file_name_am);
        free(file_name_ent);
        free(file_name_ext);
        free(file_name_bin);
        free(file_name_ob);


    }

    return 0;  /* indicating success */
}

void PrintErrorMessage(int condition, int errorMessageId) {
    if (condition) {
        fprintf(stdout, "Error: %s\n", ErrorMessages[errorMessageId]);
    }
}

