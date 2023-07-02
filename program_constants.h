/*********************************FILE__HEADER*********************************\
* File:					program_constants.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

#ifndef MAMAN14_PROGRAM_CONSTANTS_H
#define MAMAN14_PROGRAM_CONSTANTS_H

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000

typedef enum {
    IN_LABEL_OR_COMMAND, AFTER_LABEL, IN_COMMAND, AFTER_COMMAND,
    AFTER_LABEL_OR_COMMAND, IN_OPERAND, AFTER_OPERAND, EXPECTING_COMMA,
    AFTER_OPERAND_AND_WAITING
} State;

typedef struct Line {
    char **input_words;
    struct Line *next;
    int line_number;
    int num_of_words;
    int has_label;
} Line;

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

static const char *commandsList[] = {
        "mov", "cmp", "add", "sub", "not", "clr",
        "lea", "inc", "dec", "jmp", "bne", "red",
        "prn", "jsr", "rts", "stop"
};
static const int commandsListSize = sizeof(commandsList) / sizeof(char *);

static const char *instructionsList[] = {
        "data", "string", "entry", "extern"
};
static const int instructionsListSize =
        sizeof(instructionsList) / sizeof(char *);

static const char *registersList[] = {
        "r1", "r2", "r3", "r4", "r5", "r6", "r7",
};
static const int registersListSize = sizeof(registersList) / sizeof(char *);

static const OperandType operandTypes[][2] = {
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


typedef enum {
    LABEL_LENGTH_EXCEEDS_LIMIT = 0,
    LABEL_IS_RESERVED_COMMAND_WORD,
    LABEL_IS_RESERVED_INSTRUCTION_WORD,
    LABEL_IS_RESERVED_REGISTER_WORD,
    INVALID_LABEL_FORMAT,
    LABEL_MUST_START_WITH_LETTER,
    DUPLICATE_LABEL,
    LABEL_DOES_NOT_EXIST,
    ENTRY_REQUIRES_EXISTING_LABEL,
    EXTERN_REQUIRES_NONEXISTING_LABEL
} LabelErrorType;
static const char *LabelErrorMessages[] = {
        "Label length exceeds limit of 80 characters.",
        "Label '%s' is a reserved command word.",
        "Label '%s' is a reserved instruction word!",
        "Label '%s' is a reserved register word.",
        "Invalid label format for '%s'.",
        "Label must start with a letter, and the label '%s' is invalid.",
        "Duplicated label '%s'.",
        "Label '%s' does not exist neither as a label nor as an extern label.",
        "'Entry' instruction requires label that exists but Label '%s' does not exist.",
        "'Extern' instruction requires label that doesn't exist but Label '%s' already exists."
};

typedef enum {
    INCORRECT_NUM_OF_PARAMS_FOR_COMMAND = 0,
    TOO_MANY_PARAMS_FOR_COMMAND,
    INVALID_PARAM_FOR_COMMAND,
    INCORRECT_NUM_OF_PARAMS_FOR_INSTRUCTION,
    NOT_VALID_COMMAND_OR_INSTRUCTION
} CommandInstructionErrorType;
static const char *CommandInstructionErrorMessages[] = {
        "Incorrect number of parameters for command '%s'.",
        "Too many parameters for command '%s'.",
        "Invalid parameter '%s' for command '%s'.",
        "Incorrect number of parameters for instruction '%s'.",
        "Error: '%s' is not a valid command or instruction!"
};

typedef enum {
    ILLEGAL_COMMA = 0,
    MISSING_COMMA,
    MULTIPLE_CONSECUTIVE_COMMAS,
    EXTRA_COMMA_END_OF_LINE
} CommaErrorType;
static const char *CommaErrorMessages[] = {
        "Illegal comma",
        "Missing comma before char %c",
        "Multiple consecutive commas",
        "Extra comma at the end of the line"
};

typedef enum {
    NUMBER = 1,
    LABEL = 3,
    REGISTER = 5
} Types;

#endif
