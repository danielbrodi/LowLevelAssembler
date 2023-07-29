/*********************************FILE__HEADER*********************************\
* File:					am_file_processing.h
* Author:				Daniel Brodsky & Lior Katav
* Date:					August-2023
* Description:			This header file provides the interface for parsing AM
*                       files and generating binary output files.
\******************************************************************************/

#ifndef MAMAN14_AM_FILE_PROCESSING_H
#define MAMAN14_AM_FILE_PROCESSING_H

#include "utils.h" /* for the Status enum */
#include "program_constants.h" /* for the ProgramState struct */

/**
 * Parses an AM file and generates a binary output file.
 *
 * @param am_file_name The name of the input file to parse.
 * @param bin_file_name The name of the binary output file to generate.
 * @param programState A pointer to the current state of the program.
 *
 * @return Status indicating success or failure of the operation.
 */
Status
ParseFile(char *am_file_name, char *bin_file_name, ProgramState *programState);

#endif
