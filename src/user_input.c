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

void input_to_args(char* input_buffer, Args* args) {
    char *token;
    args->size = 0;

    token = strtok(input_buffer, " \n");

    /* line is blank */
    if (token == NULL)
        return;

    /* line is a comment */
    if (strncmp(token, "#", 1) == 0)
        return;

    /* line contains arguments */
    strcpy(args->items[args->size], token);
    args->size += 1;

    while (token != NULL) {
        token = strtok(NULL, " \n");
        if (token != NULL) {
            strcpy(args->items[args->size], token);
            args->size += 1;
        }
    }

}

void print_args(Args* args) {
    printf("Size of args: %i\n", args->size);
    for (int i = 0; i < args->size; i++) {
        puts(args->items[i]);
    }
}
