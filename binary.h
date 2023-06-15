/*********************************FILE__HEADER*********************************\
* File:					binary.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/
#ifndef MAMAN14_BINARY_H
#define MAMAN14_BINARY_H

#include <stdio.h>

void printBinary(int value, int numBits);

void
printBinaryCommand(int commandNum, int firstParamType, int secondParamType);

void printBinaryPrameterInteger(int number);

void printBinaryPrameterRegister(int sourceOperand, int targetOperand);

void printBinaryrPameterLabelEntry(int labelCode);

void printBinaryrPameterLabelExtern();

void printBinaryString(const char *str);

void printBinaryDataPrameter(int number);

#endif
