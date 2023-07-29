/*********************************FILE__HEADER*********************************\
*
* File:                 print_error.c
* Authors:              Daniel Brodsky & Lior Katav
* Date:                 August-2023
* Description:          Implementation of error printing utilities for an
*                       assembly language interpreter. This file includes
*                       functions for formatting and printing various types of
*                       error messages, along with string representations of
*                       those messages.
\******************************************************************************/


/******************************** Header Files ********************************/
#include <stdio.h>
#include <string.h>

#include "print_error.h"

/********************* Error Messages String Definitions **********************/
/**
 * Error message strings related to label parsing and validation.
 * Each message corresponds to a different label-related error.
 */
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

/**
 * Error message strings related to command and instruction parsing and validation.
 * Each message corresponds to a different command or instruction-related error.
 */
static const char *CommandInstructionErrorMessages[] = {
        "Incorrect number of parameters for command '%s'.",
        "Too many parameters for command '%s'.",
        "Invalid parameter '%s' for command '%s'.",
        "Incorrect number of parameters for instruction '%s'.",
        "Error: '%s' is not a valid command or instruction"
};

/**
 * Error message strings related to comma parsing and validation.
 * Each message corresponds to a different comma-related error.
 */
static const char *CommaErrorMessages[] = {
        "Illegal comma",
        "Missing comma before char %c",
        "Multiple consecutive commas",
        "Extra comma at the end of the line"
};

/************************* Functions  Implementations *************************/
/**
 * Prints an error message to stdout.
 *
 * @param lineNumber - The line where the error was encountered.
 * @param errorMessage - A string containing the error message.
 *
 * The function takes a line number and an error message as inputs and prints them to stdout.
 */
void printErrorMessage(int lineNumber, char *errorMessage) {
    fprintf(stdout, "Error on line %d: %s\n", lineNumber, errorMessage);
}
/******************************************************************************/
/**
 * Formats and prints a comma-related error message.
 *
 * @param lineNumber - The line where the error was encountered.
 * @param errorMessageId - The ID of the error message to print.
 * @param character - The character that caused the error.
 *
 * The function formats an error message based on an error ID and a character that caused the error,
 * and then calls printErrorMessage to print the formatted message.
 */
void PrintCommaErrorMessage(int lineNumber, CommaErrorType errorMessageId,
                            char character) {
    char errorMessage[256];
    sprintf(errorMessage, CommaErrorMessages[errorMessageId], character);
    printErrorMessage(lineNumber, errorMessage);
}
/******************************************************************************/
/**
 * Formats and prints a label-related error message.
 *
 * @param lineNumber - The line where the error was encountered.
 * @param errorMessageId - The ID of the error message to print.
 * @param labelName - The name of the label that caused the error.
 *
 * The function formats an error message based on an error ID and a label that caused the error,
 * and then calls printErrorMessage to print the formatted message.
 */
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
/**
 * Formats and prints a command or instruction-related error message.
 *
 * @param lineNumber - The line where the error was encountered.
 * @param errorMessageId - The ID of the error message to print.
 * @param commandOrInstructionName - The name of the command or instruction that caused the error.
 * @param additionalParam - Additional parameter involved in the error.
 *
 * The function formats an error message based on an error ID, a command or instruction name, and an additional parameter
 * that caused the error, and then calls printErrorMessage to print the formatted message.
 */
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
