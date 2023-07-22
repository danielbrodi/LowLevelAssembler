/*********************************FILE__HEADER*********************************\
* File:					binary_and_ob_processing.h
* Author:				Daniel and Lior
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_BINARY_AND_OB_PROCESSING_H
#define MAMAN14_BINARY_AND_OB_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "utils.h"
#include "program_constants.h"
#include "param_validation.h"
#include "print_error.h"

void printBinary(int value, int numBits, FILE *outputFile);

void printBinaryCommand(int commandNum, int firstParamType, int secondParamType, FILE *outputFile);

void printBinaryPrameterInteger(int number, FILE *outputFile);

void printBinaryPrameterRegister(int sourceOperand, int targetOperand, FILE *outputFile);

void printBinaryrPameterLabelEntry(int labelCode, FILE *outputFile);

void printBinaryrPameterLabelExtern(FILE *outputFile);

void printBinaryString(const char *str, FILE *outputFile);

void printBinaryDataPrameter(int number, FILE *outputFile);

int binaryToDecimal(char *binary);

char decimalToBase64(int decimal);

void binaryToBase64(const char *input_file, const char *output_file, int IC, int DC);

Status ProcessLine(Line *line, FILE *bin_fp, ProgramState *programState);

#endif

