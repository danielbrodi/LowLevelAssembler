/*********************************FILE__HEADER*********************************\
* File:					macro_processing.h
* Author:				Daniel Brodsky & Lior Katav

* Date:					August-2023
* Description:          This file provides the declarations for the preProcess
*                       function, which is used in macro processing in an
*                       assembly language program. It identifies and expands
*                       macro definitions in a given source file and writes
*                       the processed code to an output file.
\******************************************************************************/

#ifndef MACRO_PROCESSING_H
#define MACRO_PROCESSING_H

#include "program_constants.h"
#include "utils.h"

/**
 * This function performs a preprocessing step on an assembly language input file.
 * It identifies and expands macro definitions and writes the transformed code to an output file.
 *
 * @param input_file - Path to the assembly language source file.
 * @param output_file - Path to the file where the processed code will be written.
 * @param programState - Current state of the program.
 *
 * Returns SUCCESS if preprocessing completed successfully, and FAILURE otherwise.
 */
Status preProcess(const char *input_file, const char *output_file,
                  ProgramState *programState);
#endif
