/*********************************FILE__HEADER*********************************\
* File:					print_error.h
* Author:				Daniel and Lior 
 			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_PRINT_ERROR_H
#define MAMAN14_PRINT_ERROR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "program_constants.h"
#include "utils.h"

/* Error printing functions */
void printErrorMessage(int lineNumber, char *errorMessage);
void PrintCommaErrorMessage(int lineNumber, CommaErrorType errorMessageId, char character);

void PrintLabelErrorMessage(int lineNumber, LabelErrorType errorMessageId, char *labelName);

void PrintCommandInstructionErrorMessage(int lineNumber, CommandInstructionErrorType errorMessageId, char *commandOrInstructionName, char *additionalParam);

#endif
