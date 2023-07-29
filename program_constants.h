/*********************************FILE__HEADER*********************************\
* File:					program_constants.h
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			This header file includes several constants, enums, structs,
*                       and global variable definitions that are used throughout the program.
\******************************************************************************/

#ifndef MAMAN14_PROGRAM_CONSTANTS_H
#define MAMAN14_PROGRAM_CONSTANTS_H

#include "vector.h"

/* Maximum length and number of lines and labels */
#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_LINES 1000

/***************************** Enums  Definitions *****************************/
/* Different states in the parsing process */
typedef enum {
    IN_LABEL_OR_COMMAND, AFTER_LABEL, IN_COMMAND, AFTER_COMMAND,
    AFTER_LABEL_OR_COMMAND, IN_OPERAND, AFTER_OPERAND, EXPECTING_COMMA,
    AFTER_OPERAND_AND_WAITING
} State;

/* Types of operands that commands can operate on */
typedef enum {
    OPERAND_TYPE_NONE,
    OPERAND_TYPE_LABEL,
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_NUMBER,
    OPERAND_TYPE_LABEL_OR_REGISTER,
    OPERAND_TYPE_ALL
} OperandType;

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

/**************************** Structs  Definitions ****************************/
/* Struct to hold each line in the source code */
typedef struct Line {
    char **input_words;
    struct Line *next;
    int line_number;
    int num_of_words;
    int has_label;
} Line;

/* Struct to hold labels and their attributes */
typedef struct {
    char name[MAX_LABEL_LENGTH];
    int line_number;
    int asm_line_number;
    int isExtern;
    int isEntry;
} Label;

/* Struct defining the program's state. */
typedef struct {
    Vector *labels; /* Pointer to the dynamically allocated array of labels used in the program. */
    Vector *externalLabels;/* Pointer to the dynamically allocated array of external labels used in the program. */
    int current_line_number; /* Current line number being processed. */
    int IC; /* Instruction Counter - total number of instruction words in the program. */
    int DC; /* Data Counter - total number of data words in the program. */
} ProgramState;

/******************* Global variable definitions ******************************/
/* Lists of supported commands, instructions, and registers */
static const char *commandsList[] = {"mov", "cmp", "add", "sub", "not", "clr",
                                     "lea", "inc", "dec", "jmp", "bne", "red",
                                     "prn", "jsr", "rts", "stop"};
static const int commandsListSize = sizeof(commandsList) / sizeof(char *);

static const char *instructionsList[] = {"data", "string", "entry", "extern"};
static const int instructionsListSize =
        sizeof(instructionsList) / sizeof(char *);

static const char *registersList[] = {"r1", "r2", "r3", "r4", "r5", "r6", "r7"};
static const int registersListSize = sizeof(registersList) / sizeof(char *);

/* Specification of the operand types for each command */
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

/* Specification of the number of parameters for each command */
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


#endif