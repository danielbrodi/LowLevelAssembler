/*********************************FILE__HEADER*********************************\
* File:					am_file_processing.h
* Author:				Daniel and Lior										  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_AM_FILE_PROCESSING_H
#define MAMAN14_AM_FILE_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "program_constants.h"
#include "label_processing.h"
#include "binary_and_ob_processing.h"
#include "print_error.h"

Status ParseFile(char *am_file_name, char *bin_file_name, ProgramState *programState);

#endif
