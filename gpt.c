#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "macro.h"
#include "utils.h"

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000
#define INPUT_FILE_NAME "prog.as"
#define OUTPUT_FILE_NAME "prog.am"
#define RUN_PROGRAM 1
#define STOP (-1)
#define ERROR (-1)

#define START 0
#define AFTER_LABEL 1
#define IN_COMMAND 2
#define AFTER_COMMAND 3
#define IN_OPERAND 4
#define AFTER_OPERAND 5

void PrintErrorMessage(int condition, int errorMessageId);

/** @param IC A pointer to the current instruction counter
 * @param DC A pointer to the current data counter*/

typedef enum {
    OPERAND_TYPE_NONE,
    OPERAND_TYPE_VARIABLE,
    OPERAND_TYPE_VARIABLE_OR_REGISTER,
    OPERAND_TYPE_ALL
} OperandType;

typedef enum {
    SUCCESS,
    FAILURE
} Status;

typedef enum {
    FALSE,
    TRUE
} Boolean;

typedef struct {
    char name[MAX_LABEL_LENGTH];
    int line_number;
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

OperandType operandTypes[][2] = {
        {OPERAND_TYPE_ALL,      OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* mov */
        {OPERAND_TYPE_ALL,      OPERAND_TYPE_ALL},                 /* cmp */
        {OPERAND_TYPE_ALL,      OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* add */
        {OPERAND_TYPE_ALL,      OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* sub */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* not */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* clr */
        {OPERAND_TYPE_VARIABLE, OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* lea */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* inc */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* dec */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* jmp */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* bne */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* red */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_ALL},                 /* prn */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_VARIABLE_OR_REGISTER}, /* jsr */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_NONE},                /* rts */
        {OPERAND_TYPE_NONE,     OPERAND_TYPE_NONE}                 /* stop */
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

int checkLabels(char *am_file_name) {
    int i;
    FILE *file = fopen(am_file_name, "r");

    if (!file) {
        printf("File not found: %s\n", am_file_name);
        return -1;
    }

    Label labels[MAX_LINES];
    int label_count = 0;
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

int GetInput(char buffer[], FILE *file) {
    FILE *fp = fopen(OUTPUT_FILE_NAME, "r");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return STOP;
    }

    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
        /* remove trailing newline character */
        buffer[strcspn(buffer, "\n")] = 0;

        /* exits the program if entered input by the user is "stop" */
        if (strcmp("stop", buffer) == 0) {
            printf("USER CHOSE TO STOP BY COMMAND STOP, goodbye..\n");
            fclose(fp);
            return STOP;
        }
    }

    fclose(fp);
    return RUN_PROGRAM;
}

int ParseInput(char *buffer, char **input_words, int *num_of_words) {
    int state = START;
    int i = 0;

    input_words[(*num_of_words)++] = buffer;

    for (i = 0; buffer[i] != '\0'; i++) {
        char c = buffer[i];

        switch (state) {
            case START:
                if (c == ':') {
                    buffer[i] = '\0';
                    state = AFTER_LABEL;
                } else if (isspace(c)) {
                    buffer[i] = '\0';
                    state = AFTER_COMMAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, ILLEGAL_COMMA);
                    return ERROR;
                }
                break;
            case AFTER_LABEL:
            case IN_COMMAND:
                if (isspace(c)) {
                    buffer[i] = '\0';
                    state = (state == AFTER_LABEL) ? IN_COMMAND : AFTER_COMMAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, ILLEGAL_COMMA);
                    return ERROR;
                }
                break;
            case AFTER_COMMAND:
                if (isalnum(c)) {
                    input_words[(*num_of_words)++] = buffer + i;
                    state = IN_OPERAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, ILLEGAL_COMMA);
                    return ERROR;
                }
                break;
            case IN_OPERAND:
                if (c == ',') {
                    buffer[i] = '\0';
                    state = AFTER_OPERAND;
                } else if (isspace(c)) {
                    PrintErrorMessage(1, MISSING_COMMA);
                    return ERROR;
                }
                break;
            case AFTER_OPERAND:
                if (isalnum(c)) {
                    input_words[(*num_of_words)++] = buffer + i;
                    state = IN_OPERAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, EXTRA_COMMAS);
                    return ERROR;
                }
                break;
        }
    }

    if (AFTER_OPERAND == state) {
        PrintErrorMessage(1, EXTRA_TEXT);
        return ERROR;
    }

    input_words[*num_of_words] = NULL;
    for (i = 0; i < num_of_words; i++) {
        printf("Word %d is: %s.\n", i, input_words[i]);
    }
    return 0;
/*  return ProcessInput(input_words, *num_of_words); */
}

int main() {

    char user_input[MAX_LINE_LENGTH] = {0};
    int num_of_words = 0;
    char *user_command_tokens[MAX_LINE_LENGTH] = {0};
    FILE *am_file = NULL;
    int i = 0;

    am_file = preProcess(INPUT_FILE_NAME, OUTPUT_FILE_NAME);
    if (NULL == am_file) {
        return FAILURE;
    }

    checkLabels(OUTPUT_FILE_NAME);
    while (1) {
        memset(user_input, 0, sizeof(user_input));
        memset(user_command_tokens, 0, sizeof(user_command_tokens));
        num_of_words = 0;

        if (STOP == GetInput(user_input, am_file)) {
            break;
        }

        if (ERROR == ParseInput(user_input, user_command_tokens,
                                &num_of_words)) {
            printf("exiting because of error...");
            return 1;
        }
    }

    return 0;
}

void PrintErrorMessage(int condition, int errorMessageId) {
    if (condition) {
        fprintf(stdout, "Error: %s\n", ErrorMessages[errorMessageId]);
    }
}