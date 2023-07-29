/*********************************FILE__HEADER*********************************\
* File:					program_constants.h
* Author:				Daniel and lior
		 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/

#ifndef MAMAN14_PROGRAM_CONSTANTS_H
#define MAMAN14_PROGRAM_CONSTANTS_H

#include "vector.h"

/* Maximum length and number of lines and labels */
#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000

/* Different states in the parsing process */
typedef enum {
    IN_LABEL_OR_COMMAND, AFTER_LABEL, IN_COMMAND, AFTER_COMMAND,
    AFTER_LABEL_OR_COMMAND, IN_OPERAND, AFTER_OPERAND, EXPECTING_COMMA,
    AFTER_OPERAND_AND_WAITING
} State;

/* Struct to hold each line in the source code */
typedef struct Line {
    char **input_words;
    struct Line *next;
    int line_number;
    int num_of_words;
    int has_label;
} Line;

/* Types of operands that commands can operate on */
typedef enum {
    OPERAND_TYPE_NONE,
    OPERAND_TYPE_LABEL,
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_NUMBER,
    OPERAND_TYPE_LABEL_OR_REGISTER,
    OPERAND_TYPE_ALL
} OperandType;

/* Struct to hold labels and their attributes */
typedef struct {
    char name[MAX_LABEL_LENGTH];
    int line_number;
    int asm_line_number;
    int isExtern;
    int isEntry;
} Label;

/* Lists of supported commands */
static const char *commandsList[] = {
        "mov", "cmp", "add", "sub", "not", "clr",
        "lea", "inc", "dec", "jmp", "bne", "red",
        "prn", "jsr", "rts", "stop"
};
static const int commandsListSize = sizeof(commandsList) / sizeof(char *);

/* List of supported instructions */
static const char *instructionsList[] = {
        "data", "string", "entry", "extern"
};
static const int instructionsListSize =
        sizeof(instructionsList) / sizeof(char *);

/* Lists of supported registers */
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

static const int paramCount[] = {
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
    NUMBER = 1,
    LABEL = 3,
    REGISTER = 5
} Types;

/******************* Data Structures and Type Definitions ******************(**/
/* Struct defining the program's state. */
typedef struct {
    Vector *labels; /* Pointer to the dynamically allocated array of labels used in the program. */
    Vector *externalLabels;/* Pointer to the dynamically allocated array of external labels used in the program. */
    int current_line_number; /* Current line number being processed. */
    int IC; /* Instruction Counter - total number of instruction words in the program. */
    int DC; /* Data Counter - total number of data words in the program. */
} ProgramState;

#endif

