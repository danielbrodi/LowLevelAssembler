/*********************************FILE__HEADER*********************************\
* File:					create_ob.h
* Author:				Daniel Brodsky			 		  												  								
* Date:					June-2023			   								
* Description:			
\******************************************************************************/


#ifndef MAMAN14_CREATE_OB_H
#define MAMAN14_CREATE_OB_H
#include "macro.h"
#include "utils.h"
#include "binary.h"

int binaryToDecimal(char *binary);
char decimalToBase64(int decimal);

void binaryToBase64(const char *input_file, const char *output_file);
#endif
