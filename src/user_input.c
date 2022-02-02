#include "user_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_input(char* input_buffer) {
    char *raw_input;

    memset(input_buffer, '\0', MAX_CHARS*sizeof(char));
    printf(PROMPT);
    raw_input = fgets(input_buffer, MAX_CHARS, stdin);

    if (raw_input == NULL) {
        puts("Error with fgets or EOF");
    }
}

void input_to_args(char* input_buffer, RawArgs* args) {
    char *token;
    args->size = 0;

    token = strtok(input_buffer, " \n");

    /* line is blank */
    if (token == NULL)
        return;

    /* line is a comment */
    if (strncmp(token, "#", 1) == 0)
        return;

    while (token != NULL) {
        strcpy(args->items[args->size], token);
        args->size += 1;
        token = strtok(NULL, " \n");
    }

}

void print_args(RawArgs* args) {
    printf("Size of args: %i\n", args->size);
    for (int i = 0; i < args->size; i++) {
        puts(args->items[i]);
    }
}

void args_to_command(RawArgs* args, Command* cmd) {
    int cmd_args_index = 0;
    cmd->argc = 0;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->bg = 0;

    if (args->size == 0)
        return;

    cmd->file = args->items[0];

    for (int i = 0; i < args->size; i++) {

        if (strcmp(args->items[i], "<") == 0) {
            cmd->input = args->items[i + 1];
            i += 1; // skip the "<" character
            continue; // skip the input file name
        }

        if (strcmp(args->items[i], ">") == 0) {
            cmd->output = args->items[i + 1];
            i += 1; // skip the ">" character
            break; // we are at the end of the command
        }

        strcpy(cmd->argv[cmd_args_index], args->items[i]);
        cmd->argc += 1;
        cmd_args_index++;

    }

    if (strcmp(args->items[args->size - 1], "&") == 0) {
        cmd->bg = 1;
    }

}
