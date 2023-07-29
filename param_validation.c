/*********************************FILE__HEADER*********************************\
* File:                 param_validation.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          This file includes a suite of functions designed for
*                       validating various parameters and operands in an
*                       assembly language program, such as labels, registers,
*                       numbers, and instructions. It provides checks for the
*                       validity of these components, checks for their presence
*                       in the program state, and aids in determining the type
*                       of a given operand.
*
\******************************************************************************/

/******************************** Header Files ********************************/
#include <string.h>
#include <ctype.h>

#include "param_validation.h"

/************************* Functions  Implementations *************************/
/**
 * Checks if the given string is a number.
 * Negative numbers and numbers with leading '+' are considered valid.
 *
 * @param str - The string to check.
 * @return 1 if the string is a number, and 0 otherwise.
 */
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

/******************************************************************************/
/**
 * Checks if the given string is a valid register.
 * A valid register is represented as a string that starts with '@'.
 *
 * @param str - The string to check.
 * @return 1 if the string is a register, and 0 otherwise.
 */
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

/******************************************************************************/
/**
 * Checks if the given string is a valid label that exists in the program state.
 *
 * @param str - The label to check.
 * @param programState - The current program state.
 * @return 1 if the label exists in the program state, and 0 otherwise.
 */
int isLabel(const char *str, ProgramState *programState) {
    int i;
    Label *labelPtr;

    for (i = 0; i < programState->labels->size; i++) {
        labelPtr = (Label *) programState->labels->items[i];
        if (strcmp(str, labelPtr->name) == 0) {
            return 1;
        }
    }
    return 0;
}

/******************************************************************************/
/**
 * Checks if the given label exists in the program state and is not external.
 *
 * @param label - The label to check.
 * @param programState - The current program state.
 * @return TRUE if the label exists in the program state and is not external,
 *         and FALSE otherwise.
 */
Boolean isLabelExists(char *label, ProgramState *programState) {
    int i;
    Label *labelPtr;

    for (i = 0; i < programState->labels->size; i++) {
        labelPtr = (Label *) programState->labels->items[i];
        if (strcmp(label, labelPtr->name) == 0) {
            if (!labelPtr->isExtern) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/******************************************************************************/
/**
 * Returns the index of the given label in the program state's labels list.
 *
 * @param str - The label to find.
 * @param programState - The current program state.
 * @return The index of the label if found, and -1 otherwise.
 */
int getLabelIndex(const char *str, ProgramState *programState) {
    int i;
    Label *labelPtr;

    for (i = 0; i < programState->labels->size; i++) {
        labelPtr = (Label *) programState->labels->items[i];
        if (strcmp(str, labelPtr->name) == 0) {
            return i;
        }
    }
    return -1;
}

/******************************************************************************/
/**
 * Returns the index of the given instruction in the instructions list.
 *
 * @param instruction - The instruction to find.
 * @return The index of the instruction if found, and -1 otherwise.
 */
int findInstruction(const char *instruction) {
    int instructionIdx;
    const char *comparisonInstruction = instruction;

    /* If the string starts with a dot, ignore the dot for comparison purposes */
    if (instruction[0] == '.') {
        comparisonInstruction = instruction + 1;
    }

    for (instructionIdx = 0;
         instructionIdx < instructionsListSize; instructionIdx++) {
        if (strcmp(comparisonInstruction, instructionsList[instructionIdx]) ==
            0) {
            return instructionIdx;
        }
    }
    return -1;
}

/******************************************************************************/
/**
 * Returns the index of the given command in the commands list.
 * The command comparison is case-insensitive.
 *
 * @param command - The command to find.
 * @return The index of the command if found, and -1 otherwise.
 */
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

/******************************************************************************/
/**
 * Checks if the given parameter is valid for the expected operand type.
 *
 * @param param - The parameter to validate.
 * @param expectedType - The expected type of the operand.
 * @param programState - The current program state.
 * @return 1 if the parameter is valid for the expected operand type, and 0 otherwise.
 */
int isValidParam(char *param, OperandType expectedType,
                 ProgramState *programState) {
    ProgramState *currentProgramState = programState;
    switch (expectedType) {
        case OPERAND_TYPE_NONE:
            return 0;
        case OPERAND_TYPE_LABEL:
            return isLabel(param, currentProgramState);
        case OPERAND_TYPE_REGISTER:
            return isRegister(param);
        case OPERAND_TYPE_NUMBER:
            return isNumber(param);
        case OPERAND_TYPE_LABEL_OR_REGISTER:
            return isLabel(param, currentProgramState) || isRegister(param);
        case OPERAND_TYPE_ALL:
            return isLabel(param, currentProgramState) || isRegister(param) ||
                   isNumber(param);
        default:
            return 0;
    }
}

/******************************************************************************/
/**
 * Determines the type of the given operand.
 *
 * @param operand - The operand to check.
 * @param programState - The current program state.
 * @return The type of the operand if valid, and OPERAND_TYPE_NONE otherwise.
 */
int findParameterType(char *operand, ProgramState *programState) {
    ProgramState *currentProgramState = programState;
    if (isNumber(operand)) {

        return NUMBER;
    } else if (isLabel(operand, currentProgramState)) {
        return LABEL;
    } else if (isRegister(operand)) {
        return REGISTER;
    } else {
        return OPERAND_TYPE_NONE; /* not a valid type */
    }
}
/******************************************************************************/