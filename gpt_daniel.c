#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "macro.h"
#include "utils.h"
#include "binary.h"

#define DONE 2
#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000
#define INPUT_FILE_NAME "prog.as"
#define VALID_FILE_NAME "valid.as"
#define OUTPUT_FILE_NAME "prog.am"
#define AM_FILE "prog.am"
#define RUN_PROGRAM 1
#define STOP (-1)
#define ERROR (-1)

typedef enum {
    IN_LABEL_OR_COMMAND, AFTER_LABEL, IN_COMMAND, AFTER_COMMAND,
    AFTER_LABEL_OR_COMMAND, IN_OPERAND, AFTER_OPERAND, EXPECTING_COMMA,
    AFTER_OPERAND_AND_WAITING
} State;

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
int label_count = 0;

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
    EXTERN_INSTRUCTION,
    ENTRY_INSTRUCTION,
    STRING_INSTRUCTION
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

int checkLabels(char *am_file_name) {
    int i;
    FILE *file = fopen(am_file_name, "r");

    if (!file) {
        printf("File not found: %s\n", am_file_name);
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    /* First scan to validate and collect labels */
    while (fgets(line, sizeof(line), file)) {
        line_number++;

        /* Check if line starts with a label */
        char *label_end = strchr(line, ':');
        if (label_end) {
            /* Extract label */
            int label_length = label_end - line;
            /* Check if label length is less than 31 */
            if (label_length >= MAX_LABEL_LENGTH) {
                printf("Label length exceeds limit at line %d\n", line_number);
                return -1;
            }

            /* Check if label is a saved word */
            char new_label[MAX_LINE_LENGTH];
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';

            to_lowercase(new_label);

            for (i = 0; i < commandsListSize; i++) {
                if (strcmp(new_label, commandsList[i]) == 0) {
                    printf("Label is a reserved command word at line %d\n",
                           line_number);
                    return -1;
                }
            }

            for (i = 0; i < instructionsListSize; i++) {
                if (strcmp(new_label, instructionsList[i]) == 0) {
                    printf("Label is a reserved instruction word at line %d\n",
                           line_number);
                    return -1;
                }
            }

            for (i = 0; i < registersListSize; i++) {
                if (strcmp(new_label, registersList[i]) == 0) {
                    printf("Label is a reserved register word at line %d\n",
                           line_number);
                    return -1;
                }
            }

            /* Check if label is at the start of line */
            for (i = 0; i < label_length; i++) {
                if (isspace(line[i])) {
                    printf("Invalid label format at line %d\n", line_number);
                    return -1;
                }
            }

            /* Check for duplicate labels */
            strncpy(new_label, line, label_length);
            new_label[label_length] = '\0';
            for (i = 0; i < label_count; i++) {
                if (strcmp(new_label, labels[i].name) == 0) {
                    printf("Duplicate label %s at line %d\n", new_label,
                           line_number);
                    return -1;
                }
            }

            /* Copy label to labels array */
            strncpy(labels[label_count].name, new_label, label_length + 1);
            labels[label_count].line_number = line_number;
            label_count++;
        }
    }

    rewind(file);

    /* Second scan to process lines */
    while (fgets(line, sizeof(line), file)) {
        /* TODO: process each line based on your needs */
    }

    fclose(file);
    for (i = 0; i < label_count; i++) {
        printf("this is label %d: %s in line: %d\n", i, labels[i].name,
               labels[i].line_number);
    }
    return 0;
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
    int i;
    printf("CHecknig register %s!\n", str);
    for (i = 0; i < registersListSize; i++) {
        if (strcmp(str, registersList[i]) ==
            0) {
            return 1;
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

FILE *preProcess(const char *input_file, const char *output_file) {
    /* If the line starts with a macro invocation, expand it */
    Macro *macroToExpand = NULL;
    int i;
    FILE *inputFile = fopen(input_file, "r");
    if (NULL == inputFile) {
        return NULL;
    }
    FILE *outputFile = fopen(output_file, "w");
    if (NULL == outputFile) {
        return NULL;
    }
    /*TODO CHECK IF ERRORS WHILE OPEN FILES */
    char line[1024];
    Macro *currentMacro = NULL;
    MacroVector *macroVector = new_macro_vector();

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

    return outputFile;
}

int GetInput(char buffer[], FILE *fp) {
    if (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
        /* remove trailing newline character */
        buffer[strcspn(buffer, "\n")] = 0;
        return RUN_PROGRAM;
    }

    return DONE;
}

Boolean isLabelExists(char *label) {
    int labelIdx = 0;
    for (; labelIdx < label_count; labelIdx++) {
        if (strcmp(label, labels[labelIdx].name) == 0) {
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

int isValidRegister(const char *param) {
    int i;
    if (param[0] != '@') {
        return 0;
    }
    for (i = 0; i < registersListSize; i++) {
        if (strcmp(param + 1, registersList[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isValidNumber(const char *param) {
    char *endptr;
    strtol(param, &endptr, 10);
    if (*endptr == '\0') { /* Entire string is numeric */
        return 1;
    } else { /* String contains non-numeric characters */
        return 0;
    }
}

int isValidLabel(const char *param) {
    int i;
    for (i = 0; i < label_count; i++) {
        if (strcmp(param, labels[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

int isDirectRegister(char *operand) {
    if (operand[0] == '@') {
        return isRegister(operand + 1);
    }
    return 0;
}

int isValidParam(char *param, OperandType expectedType) {
    switch (expectedType) {
        case OPERAND_TYPE_NONE:
            return 0;
        case OPERAND_TYPE_LABEL:
            return isLabel(param);
        case OPERAND_TYPE_REGISTER:
            return isRegister(param) || isDirectRegister(param);
        case OPERAND_TYPE_NUMBER:
            return isNumber(param);
        case OPERAND_TYPE_LABEL_OR_REGISTER:
            return isLabel(param) || isRegister(param) ||
                   isDirectRegister(param);
        case OPERAND_TYPE_ALL:
            return isLabel(param) || isRegister(param) || isNumber(param) ||
                   isDirectRegister(param);
        default:
            return 0;
    }
}

int findParameterType(char *operand) {
    if (isNumber(operand)) {
        printf("Checknig if %s is a number and the answer is %d.\n", operand,
               isNumber(operand));
        return NUMBER;
    } else if (isLabel(operand)) {
        return LABEL;
    } else if (isRegister(operand) || isDirectRegister(operand)) {
        return REGISTER;
    } else {
        return OPERAND_TYPE_NONE; /* not a valid type */
    }
}

void UpdateLines(char *words[], int num_of_words, int has_label) {
    static int current_line_number = 100;
    int i, commandIdx, commandOrderInWords;
    char *command;

    if (has_label) {
        commandOrderInWords = 1;
    } else {
        commandOrderInWords = 0;
    }

    command = words[commandOrderInWords];
    commandIdx = findCommand(command);

    if (has_label) {
        labels[getLabelIndex(words[0])].asm_line_number = current_line_number;
    }

    if (commandIdx >= 0) { /* If it's a command and not a instruction */
        current_line_number += paramCount[commandIdx] + 1;
        /* If both parameters are registers, we decrement the line count by 1 */
        if (paramCount[commandIdx] == 2 &&
            isValidParam(words[commandOrderInWords + 1],
                         OPERAND_TYPE_REGISTER) &&
            isValidParam(words[commandOrderInWords + 2],
                         OPERAND_TYPE_REGISTER)) {
            current_line_number--;
        }
    } else { /* It's a instruction */
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

void ProcessLine(char *words[], int num_of_words, int has_label) {

    int i;
    int commandIdx = findCommand(words[has_label]);
    int instructionIdx = findInstruction(words[has_label]);
    int paramTypes[2] = {0};
    int paramType = -1;
    char *paramWords[2] = {0};
    int expectedParamCount = -1;
    int entryLabelIdx = -1;

    if (commandIdx != -1) {
        /* It's a command */
        /* Validate number of parameters */
        expectedParamCount = paramCount[commandIdx];
        if (num_of_words - 1 - has_label != expectedParamCount) {
            printf("Incorrect number of parameters for command '%s'\n",
                   words[has_label]);
            return;
        }
        /* Validate each parameter */
        int operandTypeIndex = 0;
        int paramIndex = 0;
        for (i = 1 + has_label; i < num_of_words; i++) {
            OperandType expectedType = operandTypes[commandIdx][operandTypeIndex];
            while (expectedType == OPERAND_TYPE_NONE &&
                   operandTypeIndex < 2) {
                if (paramIndex >= 2) {
                    printf("Too many parameters for command '%s'\n",
                           words[has_label]);
                    return;
                }
                paramTypes[paramIndex++] = OPERAND_TYPE_NONE;
                expectedType = operandTypes[commandIdx][++operandTypeIndex];
            }
            if (expectedType == OPERAND_TYPE_NONE) {
                printf("Too many parameters for command '%s'\n",
                       words[has_label]);
                return;
            }
            if (!isValidParam(words[i], expectedType)) {
                printf("Invalid parameter '%s' for command '%s'\n",
                       words[i], words[has_label]);
                return;
            }
            if (paramIndex >= 2) {
                printf("Too many parameters for command '%s'\n",
                       words[has_label]);
                return;
            }
            paramTypes[paramIndex] = findParameterType(words[i]);
            paramWords[paramIndex] = words[i];
            paramIndex++;
            operandTypeIndex++;
        }

        printf("The index command is %d\n", commandIdx);
        printf("The first param type is %d, the second param type is %d.\n",
               paramTypes[0], paramTypes[1]);
        printBinaryCommand(commandIdx, paramTypes[0], paramTypes[1]);

        int first_register_id = 0;
        int second_register_id = 0;

        for (i = 0; i < 2; i++) {
            paramType = paramTypes[i];
            switch (paramType) {
                case NUMBER:
                    printBinaryPrameterInteger(atoi(paramWords[i]));
                    break;
                case REGISTER: {
                    switch (i) {
                        case 0:
                            sscanf(paramWords[i], "%*[^0-9]%d",
                                   &first_register_id);
                            if (paramTypes[i + 1] == REGISTER) {
                                sscanf(paramWords[i + 1], "%*[^0-9]%d",
                                       &second_register_id);
                            }
                            break;
                        case 1:
                            if (paramTypes[i - 1] != REGISTER) {
                                sscanf(paramWords[i - 1], "%*[^0-9]%d",
                                       &first_register_id);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case LABEL:
                    printf("?\n");
                    break;
            }
        }
        if (first_register_id != 0 || second_register_id != 0) {
            printBinaryPrameterRegister(first_register_id,
                                        second_register_id);
        }
    } else if (instructionIdx != -1) {
        /* It's an instruction */
        /* printf("Debug: Processing instruction '%s'\n", words[has_label]); */
        if (instructionIdx == ENTRY_INSTRUCTION ||
            instructionIdx == EXTERN_INSTRUCTION) {
            /* For entry and extern, there should only be one parameter */
            /* printf("Debug: num_of_words = %d, has_label = %d, label = '%s'\n",
                   num_of_words, has_label, words[1 + has_label]);*/
            if (num_of_words - 1 - has_label != 1) {
                printf("Incorrect number of parameters for instruction '%s'\n",
                       words[has_label]);
                return;
            }
            /* For entry, the label must exist */
            if (instructionIdx == ENTRY_INSTRUCTION) {
                if (!isLabelExists(words[1 + has_label])) {
                    printf("Error: entry requires label that exists but Label '%s' does not exist\n",
                           words[1 + has_label]);
                    return;
                } else {
                    entryLabelIdx = getLabelIndex(words[1 + has_label]);
                }
            }
            /* For extern, the label must not exist */
            if (instructionIdx == EXTERN_INSTRUCTION) {
                if (isLabelExists(words[1 + has_label])) {
                    printf("Error: extern requires label that doesn't exist but Label '%s' already exists\n",
                           words[1 + has_label]);
                    return;
                } else {
                    /* printf("Debug: Label '%s' does not exist\n",
                            words[1 + has_label]); */
                }
            }
        } else {
            /*  printf("Debug: Not an entry or extern instruction\n"); */
        }
        switch (instructionIdx) {
            case STRING_INSTRUCTION:
                printBinaryString(words[has_label + 1]);
                break;
            case DATA_INSTRUCTION:
                for (i = 0; i < num_of_words - has_label - 1; i++) {
                    printBinaryDataPrameter(atoi(words[i]));
                }
                break;
            case EXTERN_INSTRUCTION:
                printBinaryrPameterLabelExtern();
                break;
            case ENTRY_INSTRUCTION:
                printf("?\n");
                break;
        }
    } else {
        /* printf("Debug: Not an instruction\n"); */
    }

    /* If there's a label, validate it */
    if (has_label) {
        if (!isLabel(words[0])) {
            printf("Invalid label '%s'\n", words[0]);
            return;
        }
    }

    UpdateLines(words, num_of_words, has_label);

    WriteToEntryFile(entryLabelIdx);
}

void ParseFile() {
    FILE *file = fopen("prog.am", "r");
    char buffer[256];
    char *input_words[256];
    int line_number = 0;
    State state = -1;
    int labelIdx = -1;
    int num_of_words = 0;
    int i = 0, j = 0;
    int has_label = 0; /* A flag indicating if a label exists */
    int len = -1;
    if (file == NULL) {
        printf("Failed to open file\n");
        return;
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
            switch (state) {
                case IN_LABEL_OR_COMMAND:
                    if (isspace(c)) {
                        buffer[i] = '\0';
                        state = AFTER_LABEL_OR_COMMAND;
                    } else if (c == ',') {
                        PrintErrorMessage(1, ILLEGAL_COMMA);
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
                        continue;
                    }
                    break;
                case AFTER_COMMAND:
                    if (isalnum(c) || c == '-' || c == '+') {
                        input_words[(num_of_words)++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Illegal comma\n",
                               line_number);
                        continue;
                    }
                    break;
                case IN_OPERAND:
                    if (c == ',') {
                        buffer[i] = '\0';
                        state = AFTER_OPERAND_AND_WAITING;
                    } else if (!(isalnum(c) || c == '-' || c == '+')) {
                        state = EXPECTING_COMMA;
                    }
                    break;

                case EXPECTING_COMMA:
                    if (isalnum(c)) {
                        printf("Error on line %d: Missing comma before char %c\n",
                               line_number, c);
                        buffer[i - 1] = '\0';
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        buffer[i] = '\0';
                        state = AFTER_OPERAND;
                    }
                    break;

                case AFTER_OPERAND_AND_WAITING:
                    if (isalnum(c) || c == '-' || c == '+') {
                        input_words[num_of_words++] = buffer + i;
                        state = IN_OPERAND;
                    } else if (c == ',') {
                        printf("Error on line %d: Multiple consecutive commas\n",
                               line_number);
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
                        continue;
                    }
                    break;
            }

        }
        for (i = 0; i < num_of_words; i++) {
            printf("For line %d the word idx %d is: \"%s\".\n", line_number,
                   i,
                   input_words[i]);
        }
        input_words[num_of_words] = NULL;
        ProcessLine(input_words, num_of_words, has_label);
    }

    fclose(file);
}

int main() {
    int status;
    char user_input[MAX_LINE_LENGTH] = {0};
    int num_of_words = 0;
    char *user_command_tokens[MAX_LINE_LENGTH] = {0};
    FILE *fp = NULL;
    int i = 0;

    preProcess(VALID_FILE_NAME, "asdas.am");
    fp = fopen(OUTPUT_FILE_NAME, "r");
    if (fp == NULL) {
        printf("Failed to open file in `main`\n");
        return FAILURE;
    }

    checkLabels(OUTPUT_FILE_NAME);

    ParseFile();
    for (i = 0; i < label_count; i++) {
        printf("Label name: %s, line_number: %d, asm_line_number: %d.\n",
               labels[i].name, labels[i].line_number,
               labels[i].asm_line_number);
    }
    fclose(fp);
    return 0;
}

void PrintErrorMessage(int condition, int errorMessageId) {
    if (condition) {
        fprintf(stdout, "Error: %s\n", ErrorMessages[errorMessageId]);
    }
}