#include "print_error.h"

/************************** Error Handling Functions **************************/
static const char *LabelErrorMessages[] = {
        "Label length exceeds limit of 80 characters.",
        "Label '%s' is a reserved command word.",
        "Label '.%s' is a reserved instruction word.",
        "Label '@%s' is a reserved register word.",
        "Invalid label format for '%s'.",
        "Label must start with a letter, and the label '%s' is invalid.",
        "Duplicated label '%s'.",
        "Label '%s' does not exist either as an entry label nor as an extern label.",
        "'Entry' instruction requires label that exists but Label '%s' does not exist.",
        "'Extern' instruction requires label that doesn't exist but Label '%s' already exists."
};

static const char *CommandInstructionErrorMessages[] = {
        "Incorrect number of parameters for command '%s'.",
        "Too many parameters for command '%s'.",
        "Invalid parameter '%s' for command '%s'.",
        "Incorrect number of parameters for instruction '%s'.",
        "Error: '%s' is not a valid command or instruction"
};

static const char *CommaErrorMessages[] = {
        "Illegal comma",
        "Missing comma before char %c",
        "Multiple consecutive commas",
        "Extra comma at the end of the line"
};

/******************************************************************************/
void printErrorMessage(int lineNumber, char *errorMessage) {
    fprintf(stdout, "Error on line %d: %s\n", lineNumber, errorMessage);
}
/******************************************************************************/
void PrintCommaErrorMessage(int lineNumber, CommaErrorType errorMessageId,
                            char character) {
    char errorMessage[256];
    sprintf(errorMessage, CommaErrorMessages[errorMessageId], character);
    printErrorMessage(lineNumber, errorMessage);
}
/******************************************************************************/
void PrintLabelErrorMessage(int lineNumber, LabelErrorType errorMessageId,
                            char *labelName) {
    char errorMessage[256];
    if (labelName != NULL) {
        sprintf(errorMessage, LabelErrorMessages[errorMessageId], labelName);
    } else {
        strcpy(errorMessage, LabelErrorMessages[errorMessageId]);
    }
    printErrorMessage(lineNumber, errorMessage);
}
/******************************************************************************/
void PrintCommandInstructionErrorMessage(int lineNumber,
                                         CommandInstructionErrorType errorMessageId,
                                         char *commandOrInstructionName,
                                         char *additionalParam) {
    char errorMessage[256];
    if (additionalParam != NULL) {
        sprintf(errorMessage, CommandInstructionErrorMessages[errorMessageId],
                additionalParam, commandOrInstructionName);
    } else if (commandOrInstructionName != NULL) {
        sprintf(errorMessage, CommandInstructionErrorMessages[errorMessageId],
                commandOrInstructionName);
    } else {
        strcpy(errorMessage, CommandInstructionErrorMessages[errorMessageId]);
    }
    printErrorMessage(lineNumber, errorMessage);
}
/******************************************************************************/
