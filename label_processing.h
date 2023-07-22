/*********************************FILE__HEADER*********************************\
* File:					label_processing.h
* Author:				Daniel and Lior											  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_LABEL_PROCESSING_H
#define MAMAN14_LABEL_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "program_constants.h"
#include "param_validation.h"
#include "print_error.h"

/* Label related functions */
Status checkLabels(char *am_file_name, ProgramState *programState);

void UpdateLines(char *words[], int num_of_words, int has_label, ProgramState *programState);

void WriteLabelsToFile(const char *ent_filename, const char *ext_filename,ProgramState *programState);

void addExternalLabel(int labelIdx, int lineNumber, ProgramState *programState);



#endif
