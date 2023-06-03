/*********************************FILE__HEADER*********************************\
* File:					mycomp.c
* Author:               Daniel Brodsky
* Date:					May-2023
* Description: This file encapsulates a console-based complex numbers calculator.
 * It consists of a main function that drives the program, prompting the user
 * for input, handling user commands, and performing operations on complex numbers.
 * The operations supported are addition, subtraction, multiplication, and absolute
 * value calculation, among others.
 * The file provides a user interface, takes user input and processes it.
 * Errors during the input are managed with informative error messages.
 * This application also allows user login with a customizable username.
\******************************************************************************/

/******************************** Header Files ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "complex.h"

/******************************* macros & enums *******************************/
/* colors for printing a colorful and aesthetic prompt */
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"

#define ILLEGAL_COMMA 0
#define MISSING_COMMA 1
#define EXTRA_COMMAS 2
#define EXTRA_TEXT 3
#define UNDEFINED_COMMAND 4
#define NOT_ENOUGH_PARAMS 5
#define UNDEFINED_COMPLEX_VAR 6
#define INVALID_PARAM 7

#define DEFAULT_USERNAME "Guest"

/* Constant definitions for string lengths: */
/* 1. max input of the user */
#define INPUT_LEN 256
/* 2. max characters of a username */
#define USERNAME_LEN 50
/* 3. max tokens of a user solo input : [command_name] [param1] [param2] [param3] */
#define USER_INPUT_TOKENS 50

#define RUN_PROGRAM 1
#define STOP (-1)
#define ERROR (-1)

/**
 * The following macros define the different states during command parsing:
 * - IN_COMMAND: Represents the state when currently parsing the command itself.
 * - AFTER_COMMAND: Represents the state after parsing the command and moving to the next step.
 * - IN_OPERAND: Represents the state when currently parsing an operand.
 * - AFTER_OPERAND: Represents the state after parsing an operand and moving to the next step.
 */
#define IN_COMMAND 0
#define AFTER_COMMAND 1
#define IN_OPERAND 2
#define AFTER_OPERAND 3

/**
 * The ParamType enum represents the different parameter types used in the program.
 * It includes PARAM_TYPE_COMPLEX for complex numbers and PARAM_TYPE_DOUBLE for double numbers.
 */
typedef enum {
    PARAM_TYPE_COMPLEX = 1,
    PARAM_TYPE_DOUBLE
} ParamType;

typedef void (*OperationFunction)();
/***************************** Global Definitions *****************************/
/**
 * The CommandMapping struct represents a mapping between a command name and
 * its corresponding operation function.
 */
typedef struct {
    const char *command;
    OperationFunction function;
} CommandMapping;

/**
 * This array serves as a lookup table that connects command names to the corresponding
 * functions responsible for handling the mathematical logic.
 */
CommandMapping commandMappings[] = {
        {"read_comp",      (void (*)()) read_comp},
        {"print_comp",     (void (*)()) print_comp},
        {"add_comp",       (void (*)()) add_comp},
        {"sub_comp",       (void (*)()) sub_comp},
        {"mult_comp_real", (void (*)()) mult_comp_real},
        {"mult_comp_img",  (void (*)()) mult_comp_img},
        {"mult_comp_comp", (void (*)()) mult_comp_comp},
        {"abs_comp",       (void (*)()) abs_comp},
};

int numCommands = sizeof(commandMappings) / sizeof(commandMappings[0]);

/**
 * This array contains the data type of each expected parameter and is used to validate
 * that the correct data type parameters were received for each command.
 */
ParamType paramTypes[][3] = {
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_DOUBLE, PARAM_TYPE_DOUBLE}, /* read_comp */
        {PARAM_TYPE_COMPLEX}, /* print_comp */
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_COMPLEX}, /* add_comp */
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_COMPLEX}, /* sub_comp */
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_DOUBLE}, /* mult_comp_real */
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_DOUBLE}, /* mult_comp_img */
        {PARAM_TYPE_COMPLEX, PARAM_TYPE_COMPLEX}, /* mult_comp_comp */
        {PARAM_TYPE_COMPLEX} /* abs_comp */
};


/**
 * This array is used to store the number of expected and required parameters
 * for each command and math operation.
 */
int paramCount[] = {
        3, /* read_comp */
        1, /* print_comp */
        2, /* add_comp */
        2, /* sub_comp */
        2, /* mult_comp_real */
        2, /* mult_comp_img */
        2, /* mult_comp_comp */
        1  /* abs_comp */
};

const char *ErrorMessages[] = {
        "Illegal comma.",
        "Missing comma.",
        "Multiple consecutive commas.",
        "Extraneous text after end of command.",
        "Undefined command name.",
        "Not enough parameters provided.",
        "Undefined complex variables.",
        "Invalid parameter - not a number."
};

/**************************** Forward Declarations ****************************/
void printMenu();

int GetInput(char buffer[]);

int ParseInput(char buffer[], char *input_words[], int *num_of_words);

int ProcessInput(char *words[], int num_of_words);

Complex runOperation(char *command_name, char **params, int num_of_params,
                     Complex *A, Complex *B, Complex *C, Complex *D, Complex *E,
                     Complex *F);

int isValidParam(const char *param, ParamType type);

int validate_params(int command, const char **argv, int num_of_params);

int isValidComplexVariable(const char *variable);

int isValidDoubleVariable(const char *param);

OperationFunction isCommandValid(const char *command);

int countParams(int num_of_words);

int isEnoughParams(const char *command, int num_of_words);

int getExpectedParams(const char *command);

void PrintErrorMessage(int condition, int errorMessageId);
/******************************************************************************/
/******************************* Main  Function *******************************/
int main() {
    /* User input and username arrays */
    char user_input[INPUT_LEN] = {0}, username[USERNAME_LEN] = {0};

    /* Variables for parsed user input */
    int num_of_words = 0;
    char *user_command_tokens[USER_INPUT_TOKENS] = {0};

    /* request and read in the username */
    printf(KYEL "Please enter a username to log in with: " KCYN);
    fgets(username, USERNAME_LEN - 2, stdin);

    /* remove newline character from username */
    username[strcspn(username, "\n")] = 0;

    /* check if username is blank and set it to "Guest" if it is */
    if (username[0] == '\0') {
        strncpy(username, DEFAULT_USERNAME, sizeof(username) - 1);
    }
    /* confirmation of successful login */
    printf(KBLU "\n\n* Successfully logged in\n" KNRM);

    /* display the menu with the instructions of the program */
    printMenu();

    /* Main loop for reading and processing user input */
    while (RUN_PROGRAM) {

        /* Print command prompt with username */
        printf(KGRN">>%s@DB-COMPLEX-Calculator"KNRM": ", username);

        /* Reset user input and user command tokens before each new command */
        memset(user_input, 0, sizeof(user_input));
        memset(user_command_tokens, 0, sizeof(user_command_tokens));
        num_of_words = 0;

        /* Get user input. If input indicates to stop, break out of loop */
        if (STOP == GetInput(user_input)) {
            break;
        }

        /** Parse user input into command tokens and get number of words.
            * in case of an error in the input: the inserted input was formatted in a wrong way,
            * the loop will continue but discard the request operation */
        if (ERROR == ParseInput(user_input, user_command_tokens,
                                &num_of_words)) {
            continue;
        };

        /* Run operation based on user command and get result */
        result = runOperation(user_command_tokens[0], user_command_tokens,
                              num_of_words, &A, &B, &C, &D, &E, &F);

        /* If user command was "read_comp", print the name of the complex number */
        if (strcmp(user_command_tokens[0], "read_comp") == 0) {
            printf("%s = ", user_command_tokens[1]);
        }

        /* Print the resulting complex number */
        print_comp(&result);
    }

    /* end program */
    return 0;
}

/*********************** Helper Functions  Implementations ********************/
/**
 * Reads user input from the standard input (stdin), removes trailing newline characters,
 * and checks if the user input is a stop command.
 *
 * The function also handles the end-of-file (EOF) condition, which may occur if the
 * user tries to close the program in an invalid way. In such a case, it notifies
 * the user with an appropriate message.
 *
 * buffer: An array of characters that holds the user input string.
 *
 * returns: Integer value indicating whether the program should stop or continue running.
 *          The function returns 'STOP' if the user input is "stop" or if EOF is encountered.
 *          Otherwise, it returns 'RUN_PROGRAM'.
 */
int GetInput(char buffer[]) {
    do {
        if (fgets(buffer, INPUT_LEN, stdin) == NULL) {
            /*	exits the program if entered character by the user is EOF	*/
            printf("Stopped the program in an invalid way, you should use the `stop` command.\n");
            return STOP;
        }

        /* remove trailing newline character */
        buffer[strcspn(buffer, "\n")] = 0;

        /* exits the program if entered input by the user is "stop" */
        if (strcmp("stop", buffer) == 0) {
            printf("Program will now exit...\n");
            return STOP;
        }

    } while (buffer[0] == '\0');

    return RUN_PROGRAM;
}
/******************************************************************************/
/**
 * Parses the input string from the user into individual words based on whitespace and commas.
 * The function also checks for proper syntax and structure of the input and returns an error
 * if the input string is invalid.
 *
 * The input string can have multiple parts separated by whitespace or commas, representing a command
 * and its operands. The function transitions through different states depending on the current
 * character being parsed:
 * - IN_COMMAND: Reading the command portion of the input. Any comma found in this state is treated
 *               as an error.
 * - AFTER_COMMAND: After reading the command, expecting the start of an operand or another command.
 *                  Any comma found in this state is treated as an error.
 * - IN_OPERAND: Reading an operand of a command. Any whitespace found in this state is treated as
 *               an error and the function expects a comma as the separator.
 * - AFTER_OPERAND: After reading an operand, expecting the start of another operand. Any comma
 *                  found in this state is treated as an error.
 *
 * buffer: The input string from the user.
 *
 * input_words: An array of character pointers to store the individual words from the input string.
 *              Each word will point to the start of the word in the buffer.
 *
 * num_of_words: The number of words found in the input string.
 *
 * returns: An integer value indicating the status of the input parsing and processing. It returns
 *          'ERROR' if there is any syntax or structural issue in the input string. Otherwise,
 *          it calls the function ProcessInput() with the parsed words and their count, and returns
 *          the result of ProcessInput().
 */
int ParseInput(char *buffer, char **input_words, int *num_of_words) {
    int state = IN_COMMAND;
    int i = 0;

    input_words[(*num_of_words)++] = buffer;

    for (i = 0; buffer[i] != '\0'; i++) {
        char c = buffer[i];

        switch (state) {
            case IN_COMMAND:
                if (isspace(c)) {
                    buffer[i] = '\0';
                    state = AFTER_COMMAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, ILLEGAL_COMMA);
                    return ERROR;
                }
                break;
            case AFTER_COMMAND:
                if (isalnum(c)) {
                    input_words[(*num_of_words)++] = buffer + i;
                    state = IN_OPERAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, ILLEGAL_COMMA);
                    return ERROR;
                }
                break;
            case IN_OPERAND:
                if (c == ',') {
                    buffer[i] = '\0';
                    state = AFTER_OPERAND;
                } else if (isspace(c)) {
                    PrintErrorMessage(1, MISSING_COMMA);
                    return ERROR;
                }
                break;
            case AFTER_OPERAND:
                if (isalnum(c)) {
                    input_words[(*num_of_words)++] = buffer + i;
                    state = IN_OPERAND;
                } else if (c == ',') {
                    PrintErrorMessage(1, EXTRA_COMMAS);
                    return ERROR;
                }
                break;
        }
    }

    if (AFTER_OPERAND == state) {
        PrintErrorMessage(1, EXTRA_TEXT);
        return ERROR;
    }

    input_words[*num_of_words] = NULL;
    return ProcessInput(input_words, *num_of_words);
}
/******************************************************************************/
/**
 * Processes the parsed input words from the user. The first word is assumed to be a command,
 * and the rest of the words are considered as parameters of that command.
 *
 * This function checks if the command is valid by calling isCommandValid(). If the command
 * is invalid, it prints an error message and returns an error code.
 *
 * The function also checks if the number of parameters is enough for the command by calling
 * isEnoughParams(). If the number of parameters is insufficient, it prints an error message
 * and returns an error code.
 *
 * If the command is valid and the number of parameters is sufficient, it validates the parameters
 * using validate_params(). If the parameters are invalid, it prints an error message and
 * returns an error code.
 *
 * words: An array of character pointers, each pointing to a parsed word from the input string.
 *
 * num_of_words: The number of words in the words array.
 *
 * returns: An integer value indicating the status of the input processing. It returns 'ERROR'
 *          if the command is invalid, if the number of parameters is insufficient, or if the
 *          parameters are invalid. Otherwise, it returns 0 indicating successful processing.
 */
int ProcessInput(char *words[], int num_of_words) {
    int commandIdx = -1;
    int i;
    char *command = NULL;
    int num_of_params = -1;
    int valid = 0;
    OperationFunction operation = NULL;

    command = words[0];
    operation = isCommandValid(command);
    if (NULL == operation) {
        PrintErrorMessage(1, UNDEFINED_COMMAND);
        return ERROR;
    }
    num_of_params = countParams(num_of_words);
    if (!isEnoughParams(command, num_of_words)) {
        PrintErrorMessage(1, NOT_ENOUGH_PARAMS);
        return ERROR;
    }

    for (i = 0; i < numCommands; i++) {
        if (commandMappings[i].function == operation) {
            commandIdx = i;
            break;
        }
    }
    valid = validate_params(commandIdx, (const char **) words, num_of_params);
    if (valid != 0) {
        PrintErrorMessage(1, valid == PARAM_TYPE_COMPLEX ? UNDEFINED_COMPLEX_VAR
                                                         : INVALID_PARAM);
        return ERROR;
    }

    return 0;
}
/******************************************************************************/
/**
 * Checks if the given command string matches any of the predefined valid commands.
 *
 * The function iterates through an array of command mappings, each containing a command
 * string and its corresponding operation function. If it finds a match for the input
 * command, it returns the associated operation function.
 *
 * command: The command string to validate.
 *
 * returns: The operation function associated with the valid command. If the command is not
 *          valid (i.e., there is no matching command in the predefined set), it returns NULL.
 */
OperationFunction isCommandValid(const char *command) {
    int commandIdx = -1;
    for (commandIdx = 0; commandIdx < numCommands; commandIdx++) {
        if (strcmp(command, commandMappings[commandIdx].command) == 0) {
            return commandMappings[commandIdx].function;
        }
    }
    return NULL;
}
/******************************************************************************/
/**
 * Determines the expected number of parameters for a given command.
 *
 * This function checks the command string against a predefined set of valid commands.
 * If the command matches one of these, it returns the number of parameters that command is expected to have.
 *
 * If the command does not match any of the predefined commands, it returns -1 to indicate an invalid command.
 *
 * command: The command string for which to determine the expected number of parameters.
 *
 * returns: The number of parameters expected by the command, or -1 if the command is not recognized.
 */
int getExpectedParams(const char *command) {
    if (strcmp(command, "read_comp") == 0) {
        return 3;
    } else if (strcmp(command, "print_comp") == 0) {
        return 1;
    } else if (strcmp(command, "add_comp") == 0) {
        return 2;
    } else if (strcmp(command, "sub_comp") == 0) {
        return 2;
    } else if (strcmp(command, "mult_comp_real") == 0) {
        return 2;
    } else if (strcmp(command, "mult_comp_img") == 0) {
        return 2;
    } else if (strcmp(command, "mult_comp_comp") == 0) {
        return 2;
    } else if (strcmp(command, "abs_comp") == 0) {
        return 1;
    }
    return ERROR; /* invalid command */
}
/******************************************************************************/
/**
 * Validates the parameters of a given command.
 *
 * This function uses the command index to lookup the expected parameter types and their count.
 * It then iterates through each parameter, checking if its type matches the expected type.
 * The validation is done using the isValidParam() function.
 *
 * command: The index of the command in the paramTypes and paramCount arrays.
 *
 * argv: An array of strings, each string being a parameter for the command.
 *
 * num_of_params: The number of parameters in the argv array.
 *
 * returns: The expected type if a parameter is found to be invalid. Otherwise, it returns 0
 *          if all parameters are valid. Note: this function doesn't currently have a return
 *          statement, so you might want to add a "return 0;" at the end to indicate successful validation.
 */
int validate_params(int command, const char **argv, int num_of_params) {
    const ParamType *expected_types = paramTypes[command];
    int i;
    /* Get the expected number of parameters for the command from the paramTypes array */
    int expected_num_params = paramCount[command];

    /* Iterate only up to the smallest of num_of_params or expected_num_params */
    for (i = 1; i <= expected_num_params && i <= num_of_params; i++) {
        if (!isValidParam(argv[i], expected_types[i - 1])) {
            return expected_types[i - 1];
        }
    }

    /* Check if the number of parameters provided matches the expected number */
    if (num_of_params > expected_num_params) {
        PrintErrorMessage(1, EXTRA_TEXT);
        return ERROR;
    }

    return 0; /* params are valid */
}
/******************************************************************************/
/**
 * Checks if the number of parameters for a given command is sufficient.
 *
 * This function first retrieves the expected number of parameters for the command using the
 * getExpectedParams() function. If the command is unrecognized (expected_params equals -1),
 * it prints an error message and returns 0, indicating that the parameters are not sufficient.
 *
 * If the command is recognized, it compares the actual number of words in the input with the
 * expected number of parameters (plus one to account for the command itself).
 * If the actual number is greater than or equal to the expected number, it returns 1 to indicate
 * that there are enough parameters. Otherwise, it returns 0.
 *
 * command: The command string to check for parameter sufficiency.
 *
 * num_of_words: The actual number of words in the input (including the command).
 *
 * returns: 1 if the number of parameters is sufficient, 0 otherwise.
 */
int isEnoughParams(const char *command, int num_of_words) {
    int expected_params = getExpectedParams(command);
    if (expected_params == -1) {
        PrintErrorMessage(1, UNDEFINED_COMMAND);
        return 0;
    }
    return num_of_words >= expected_params + 1;
}

/******************************************************************************/
int countParams(int num_of_words) {
    int count = num_of_words - 1;
    return count;
}
/******************************************************************************/
/**
 * Checks if a parameter is valid based on its expected type.
 *
 * This function compares the expected parameter type with predefined types.
 * If the expected type is a complex variable, it checks if the parameter is a valid complex variable.
 * If the expected type is a double variable, it checks if the parameter is a valid double variable.
 * The parameter validation is done by calling the respective functions isValidComplexVariable()
 * and isValidDoubleVariable().
 *
 * If the expected type does not match any of the predefined types, the function returns 0
 * indicating an invalid parameter type.
 *
 * param: The parameter to validate.
 *
 * type: The expected type of the parameter.
 *
 * returns: 1 if the parameter is valid, 0 otherwise.
 */
int isValidParam(const char *param, ParamType type) {
    if (type == PARAM_TYPE_COMPLEX) {
        return isValidComplexVariable(param);
    } else if (type == PARAM_TYPE_DOUBLE) {
        return isValidDoubleVariable(param);
    }

    return 0; /* Invalid parameter type */
}

/******************************************************************************/
int isValidComplexVariable(const char *variable) {
    if (strlen(variable) != 1) {
        return 0;
    }
    return (variable[0] >= 'A' && variable[0] <= 'F');
}

/******************************************************************************/
int isValidDoubleVariable(const char *param) {
    char *endptr;
    strtod(param, &endptr);

    if (*endptr != '\0' || endptr == param) {
        return 0;
    }

    return 1;
}
/******************************************************************************/
/**
 * Executes a specified operation on complex variables.
 *
 * This function runs the operation given by command_name on the specified parameters.
 * The parameters should be complex variable names ("A", "B", "C", etc.), or in the case of
 * the read_comp operation, two floating point numbers.
 *
 * This function also prints out a description of the operation to be performed, and the
 * parameters it will be performed on.
 *
 * command_name: The name of the command to be executed.
 *
 * params: An array of strings containing the parameters for the command.
 *
 * num_of_params: The number of parameters in the params array.
 *
 * A, B, C, D, E, F: Pointers to the complex variables that the operations can be performed on.
 *
 * returns: The result of the operation, as a Complex struct. The actual meaning of this result
 *          depends on the operation performed. For read_comp, it is the complex variable that was read.
 *          For print_comp, it is the complex variable that was printed. For the arithmetic operations,
 *          it is the result of the operation. For abs_comp, it is a complex number whose real part is
 *          the absolute value of the specified complex variable, and whose imaginary part is 0.
 */
Complex runOperation(char *command_name, char **params, int num_of_params,
                     Complex *A, Complex *B, Complex *C, Complex *D, Complex *E,
                     Complex *F) {
    int i;
    Complex result = {0};
    Complex *complex_params[2] = {0};

    for (i = 1; i < num_of_params && i - 1 < 2; i++) {
        switch (params[i][0]) {
            case 'A':
                complex_params[i - 1] = A;
                break;
            case 'B':
                complex_params[i - 1] = B;
                break;
            case 'C':
                complex_params[i - 1] = C;
                break;
            case 'D':
                complex_params[i - 1] = D;
                break;
            case 'E':
                complex_params[i - 1] = E;
                break;
            case 'F':
                complex_params[i - 1] = F;
                break;
        }
    }

    printf("You have asked to run operation '%s' with the params: ",
           command_name);
    for (i = 1; i < num_of_params - 1; i++) {
        printf("%s, ", params[i]);
    }
    printf("%s.\n", params[i]);
    if (strcmp(command_name, "read_comp") == 0) {
        read_comp(complex_params[0], atof(params[2]), atof(params[3]));
        result = *complex_params[0];
    } else if (strcmp(command_name, "print_comp") == 0) {
        printf("print_comp was ordered for: '%s'\n", params[1]);
        result = *complex_params[0];
    } else if (strcmp(command_name, "add_comp") == 0) {
        printf("'%s' and '%s'\n", params[1], params[2]);
        result = add_comp(complex_params[0], complex_params[1]);
    } else if (strcmp(command_name, "sub_comp") == 0) {
        printf("'%s' and '%s'\n", params[1], params[2]);
        result = sub_comp(complex_params[0], complex_params[1]);
    } else if (strcmp(command_name, "mult_comp_real") == 0) {
        printf("'%s' and '%s'\n", params[1], params[2]);
        result = mult_comp_real(complex_params[0], atof(params[2]));
    } else if (strcmp(command_name, "mult_comp_img") == 0) {
        printf("'%s' and '%s'\n", params[1], params[2]);
        result = mult_comp_img(complex_params[0], atof(params[2]));
    } else if (strcmp(command_name, "mult_comp_comp") == 0) {
        printf("'%s' and '%s'\n", params[1], params[2]);
        result = mult_comp_comp(complex_params[0], complex_params[1]);
    } else if (strcmp(command_name, "abs_comp") == 0) {
        printf("'%s'\n", params[1]);
        read_comp(&result, abs_comp(complex_params[0]), 0);
    }
    return result;
}
/******************************************************************************/
/**
 * Prints an error message if a certain condition is met.
 *
 * This function checks a condition and if it's true, it prints an error message to stdout.
 * The error message is indexed from a predefined array of error messages `ErrorMessages[]`
 * using an error message ID.
 *
 * condition: The condition to check. If this condition is true (non-zero), an error message is printed.
 *
 * errorMessageId: The index of the error message in the `ErrorMessages[]` array.
 *
 */
void PrintErrorMessage(int condition, int errorMessageId) {
    if (condition) {
        fprintf(stdout, "Error: %s\n", ErrorMessages[errorMessageId]);
    }
}

/******************************************************************************/
void printMenu() {
    printf("\nAvailable commands:\n\n");
    printf("1. read_comp: Assign a complex number value to a complex variable.\n");
    printf("   Example: read_comp A, 1.0, 2.0\n");
    printf("2. print_comp: Print a complex number's value.\n");
    printf("   Example: print_comp A\n");
    printf("3. add_comp: Add two complex numbers.\n");
    printf("   Example: add_comp A, B\n");
    printf("4. sub_comp: Subtract two complex numbers.\n");
    printf("   Example: sub_comp A, B\n");
    printf("5. mult_comp_real: Multiply a complex number by a real number.\n");
    printf("   Example: mult_comp_real A, 2.0\n");
    printf("6. mult_comp_img: Multiply a complex number by an imaginary number.\n");
    printf("   Example: mult_comp_img A, 2.0\n");
    printf("7. mult_comp_comp: Multiply two complex numbers.\n");
    printf("   Example: mult_comp_comp A, B\n");
    printf("8. abs_comp: Compute the absolute value of a complex number.\n");
    printf("   Example: abs_comp A\n");
    printf("9. stop: Stop the program.\n");
    printf("   Example: stop\n");
    printf("\nNote: Please separate each command and its parameters with commas (','). \n");
    printf("* The only exception is the first word, which represents the command itself.\n");
    printf("\nNote: The only valid complex number variables are A, B, C, D, E, F.\n");
}
/******************************************************************************/