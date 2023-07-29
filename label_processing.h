/*********************************FILE__HEADER*********************************\
* File:                  label_processing.h
* Authors:               Daniel Brodsky & Lior Katav
* Date:                  August-2023
* Description:           This header file provides an interface for the label processing
*                        functionality in an assembly language program. It contains the
*                        function prototypes for checking labels, updating lines in
*                        the program, writing labels to files, and adding external labels.
\******************************************************************************/
#ifndef MAMAN14_LABEL_PROCESSING_H
#define MAMAN14_LABEL_PROCESSING_H

#include "program_constants.h"
#include "utils.h"

/**
 * Checks all labels in the assembly file for syntax and logical errors.
 * Returns a Status indicating whether the operation was successful or not.
 */
Status checkLabels(char *am_file_name, ProgramState *programState);

/**
 * Processes labels and lines of assembly code to update line numbers and
 * corresponding label states in the program. This includes managing references to
 * labels and handling line adjustments for label usage.
 * The function doesn't return a value.
 */
void UpdateLines(char *words[], int num_of_words, int has_label,
                 ProgramState *programState);

/**
 * Writes all labels into entry and external files.
 * This function does not return a value.
 */
void WriteLabelsToFile(const char *ent_filename, const char *ext_filename,
                       ProgramState *programState);

#endif
