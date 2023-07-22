/*********************************FILE__HEADER*********************************\
* File:					param_validation.h
* Author:				Daniel and Lior											  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef PARAM_VALIDATION_H
#define PARAM_VALIDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "program_constants.h"

int isLabel(const char *str, ProgramState *programState);

int getLabelIndex(const char *str, ProgramState *programState);

Boolean isLabelExists(char *label, ProgramState *programState);

int isNumber(const char *str);

int isRegister(const char *str);

int findInstruction(const char *instruction);

int findCommand(char *command);

int isValidParam(char *param, OperandType expectedType, ProgramState *programState);

int findParameterType(char *operand, ProgramState *programState);

#endif
