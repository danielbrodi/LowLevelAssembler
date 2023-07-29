/*********************************FILE__HEADER*********************************\
* File:                 param_validation.h
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          This header file provides the declarations for a suite of
*                       functions designed for validating various parameters and
*                       operands in an assembly language program. It aids in
*                       checking the validity of labels, registers, numbers,
*                       and instructions, checks their presence in the program
*                       state, and assists in determining the type of a given
*                       operand.
\******************************************************************************/

#ifndef PARAM_VALIDATION_H
#define PARAM_VALIDATION_H

#include "utils.h"
#include "program_constants.h"

/* Function that checks if a given string is a label in the program state */
int isLabel(const char *str, ProgramState *programState);

/* Function that returns the index of a given label in the program state */
int getLabelIndex(const char *str, ProgramState *programState);

/* Function that checks if a given label exists in the program state and isn't external */
Boolean isLabelExists(char *label, ProgramState *programState);

/* Function that checks if a given string is a number */
int isNumber(const char *str);

/* Function that checks if a given string is a register */
int isRegister(const char *str);

/* Function that finds the index of a given instruction in the instruction list */
int findInstruction(const char *instruction);

/* Function that finds the index of a given command in the commands list */
int findCommand(char *command);

/* Function that validates if a given parameter is of the expected type */
int
isValidParam(char *param, OperandType expectedType, ProgramState *programState);

/* Function that determines the type of a given operand */
int findParameterType(char *operand, ProgramState *programState);

#endif