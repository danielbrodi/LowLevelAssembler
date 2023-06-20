/*********************************FILE__HEADER*********************************\
* File:					binary.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_BINARY_H
#define MAMAN14_BINARY_H

#include <stdio.h>

void printBinary(int value, int numBits, FILE *outputFile);

void printBinaryCommand(int commandNum, int firstParamType, int secondParamType, FILE *outputFile);

void printBinaryPrameterInteger(int number, FILE *outputFile);

void printBinaryPrameterRegister(int sourceOperand, int targetOperand, FILE *outputFile);

void printBinaryrPameterLabelEntry(int labelCode, FILE *outputFile);

void printBinaryrPameterLabelExtern(FILE *outputFile);

void printBinaryString(const char *str, FILE *outputFile);

void printBinaryDataPrameter(int number, FILE *outputFile);

#endif
