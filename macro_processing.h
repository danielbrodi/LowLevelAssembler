/*********************************FILE__HEADER*********************************\
* File:					pre_process.h
* Author:				Daniel and Lior 
		 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MACRO_PROCESSING_H
#define MACRO_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "program_constants.h"
#include "utils.h"
#include "param_validation.h"
#include "macro_vector.h"

/* File processing and parsing functions */
Status preProcess(const char *input_file, const char *output_file, ProgramState *programState);

int isValidMacroDefinition(const char *line);

int isReservedKeyword(char *word, ProgramState *programState);

#endif
