#include "param_validation.h"

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
int isLabel(const char *str, ProgramState *programState) {
    int i;
    for (i = 0; i < programState->label_count; i++) {
        if (strcmp(str, programState->labels[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

/******************************************************************************/
Boolean isLabelExists(char *label, ProgramState *programState) {
    if (isLabel(label, programState)) {
        if (!programState->labels[getLabelIndex(label,
                                                programState)].isExtern) {
            return TRUE;
        }
    }
    return FALSE;
}

/******************************************************************************/
int getLabelIndex(const char *str, ProgramState *programState) {
    int i;
    for (i = 0; i < programState->label_count; i++) {
        if (strcmp(str, programState->labels[i].name) == 0) {
            return i;
        }
    }
    return -1;
}
/******************************************************************************/
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

/******************************************************************************/
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
int findParameterType(char *operand, ProgramState *programState) {
    ProgramState *currentProgramState = programState;
    if (isNumber(operand)) {
        /*
        printf("Checking if %s is a number and the answer is %d.\n", operand,
               isNumber(operand));
               */
        return NUMBER;
    } else if (isLabel(operand, currentProgramState)) {
        return LABEL;
    } else if (isRegister(operand)) {
        return REGISTER;
    } else {
        return OPERAND_TYPE_NONE; /* not a valid type */
    }
}
