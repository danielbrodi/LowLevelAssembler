/*********************************FILE__HEADER*********************************\
* File:					binary_and_ob_processing.h
* Author:				Daniel Brodsky & Lior Katav
* Date:				    August-2023
* Description:          This header file contains function declarations
*                       necessary for binary and .ob file processing for
*                       our custom compiler. This includes
*                       functions to process each line of assembly input and
*                       generate appropriate binary output and also for
*                       conversion of binary output to base64 format.
\******************************************************************************/
#ifndef MAMAN14_BINARY_AND_OB_PROCESSING_H
#define MAMAN14_BINARY_AND_OB_PROCESSING_H

#include <stdio.h>

#include "utils.h" /* for the Status enum */
#include "program_constants.h" /* for the ProgramState struct */

/**
 * Converts binary data from a given file to Base64 format, and writes it to
 * an output file.
 *
 * @param input_file The path of the file with binary data.
 * @param output_file The path of the file to write the Base64 output.
 * @param IC Instruction counter, represents the amount of instructions.
 * @param DC Data counter, represents the amount of data.
 */
void
binaryToBase64(const char *input_file, const char *output_file, int IC, int DC);

/**
 * Processes a line of assembly code, interprets the commands, parameters, and
 * labels, and writes the equivalent binary code to a binary output file.
 *
 * @param line A pointer to the line to process.
 * @param bin_fp A pointer to the file to write the binary output.
 * @param programState A pointer to the ProgramState structure with the current
 *                     state of the program.
 *
 * @return Status indicating success or failure of the operation.
 */
Status ProcessLine(Line *line, FILE *bin_fp, ProgramState *programState);

#endif

