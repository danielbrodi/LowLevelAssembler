#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int GetInput(char buffer[]);

int isCommandMacro(char *command);

int ParseInput(char *buffer, char **input_words, int *num_of_words);

#define IN_COMMAND 0
#define AFTER_COMMAND 1
#define IN_OPERAND 2
#define AFTER_OPERAND 3

int main() {
    char user_input[INPUT_LEN] = {0};

    int num_of_words = 0;
    char *user_command_tokens[USER_INPUT_TOKENS] = {0};

    while (1) {

        memset(user_input, 0, sizeof(user_input));
        memset(user_command_tokens, 0, sizeof(user_command_tokens));
        num_of_words = 0;

        if (STOP == GetInput(user_input)) {
            break;
        }

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
int ProcessInput(char *words[], int num_of_words) {
    int commandIdx = -1;
    int i;
    char *command = NULL;
    int num_of_params = -1;
    int isMacro = 0;
    char *macro_name = NULL;
    OperationFunction operation = NULL;

    command = words[0];
    isMacro = isCommandMacro(words[0]);
    if (isMacro) {
        macro_name = words[1];
    }
}

int isCommandMacro(char *command) {
    if (0 == strcmp(command, "mcro") {
        printf("this is macro.\n")
        return 1;
    } else if (0 == strcmp(command, "endmcro")) {
        printf("this is end macro.\n");
        return 1;
    }
    return 0;
}