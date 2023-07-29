/*********************************FILE__HEADER*********************************\
* File:					print_error.h
* Author:				Daniel and Lior
* Date:					June-2023
* Description:			This header file includes definitions of various enums used for
*                       identifying types of errors in our compiler. It also
*                       declares several functions used to print error messages based on these error types.
\******************************************************************************/

#ifndef MAMAN14_PRINT_ERROR_H
#define MAMAN14_PRINT_ERROR_H

/**
 * An enum of types of errors that can occur with labels in assembly code.
 */
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


/**
 * An enum of types of errors that can occur with command and instruction
 * parsing and validation in assembly code.
 */
typedef enum {
    INCORRECT_NUM_OF_PARAMS_FOR_COMMAND = 0,
    TOO_MANY_PARAMS_FOR_COMMAND,
    INVALID_PARAM_FOR_COMMAND,
    INCORRECT_NUM_OF_PARAMS_FOR_INSTRUCTION,
    NOT_VALID_COMMAND_OR_INSTRUCTION
} CommandInstructionErrorType;

/**
 * An enum of types of errors that can occur with comma usage in assembly code.
 */
typedef enum {
    ILLEGAL_COMMA = 0,
    MISSING_COMMA,
    MULTIPLE_CONSECUTIVE_COMMAS,
    EXTRA_COMMA_END_OF_LINE
} CommaErrorType;

/**
 * Function to print an error message based on a given CommaErrorType.
 */
void PrintCommaErrorMessage(int lineNumber, CommaErrorType errorMessageId,
                            char character);

/**
 * Function to print an error message based on a given LabelErrorType.
 */
void PrintLabelErrorMessage(int lineNumber, LabelErrorType errorMessageId,
                            char *labelName);

/**
 * Function to print an error message based on a given CommandInstructionErrorType.
 */
void PrintCommandInstructionErrorMessage(int lineNumber,
                                         CommandInstructionErrorType errorMessageId,
                                         char *commandOrInstructionName,
                                         char *additionalParam);

#endif
