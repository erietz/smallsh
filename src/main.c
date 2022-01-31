#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512
#define PROMPT ": "

typedef struct Args {
    int size;
    char items[MAX_ARGS][MAX_CHARS];
} Args;


void read_input(char* input_buffer) {
    char *raw_input;

    memset(input_buffer, '\0', MAX_CHARS*sizeof(char));
    printf(PROMPT);
    raw_input = fgets(input_buffer, MAX_CHARS, stdin);    // TODO: cannot read passed \n char

    if (raw_input == NULL) {
        puts("Error with fgets or EOF");
    }
}

void parse_input(char* input_buffer, Args* args) {
    char *token;
    /* char *current_arg; */
    args->size = 0;

    token = strtok(input_buffer, " \n");
    /* current_arg = strdup(token); */
    strcpy(args->items[args->size], token);
    args->size += 1;

    while (token != NULL) {
        token = strtok(NULL, " \n");
        if (token != NULL) {
            /* current_arg = strdup(token); */
            /* args->items[args->size] = current_arg; */
            strcpy(args->items[args->size], token);
            args->size += 1;
        }
    }

    /* TODO: If using fgets this is unnessary since I can use " \n" in strtok */
    /* // trim off trailing new line from last item in args array */
    /* int len = strlen(current_arg); */
    /* if (current_arg[len - 1] == '\n') { */
    /*     current_arg[len - 1] = '\0'; */
    /* } */

}

void print_args(Args* args) {
    for (int i = 0; i < args->size; i++) {
        puts(args->items[i]);
    }
}

void free_args(Args* args) {
    for (int i = 0; i < args->size; i++) {
        free(args->items[i]);
    }
    /* free(args->items); */
    /* free(args); */
}

int main(int argc, char *argv[]) {
    char input_buffer[MAX_CHARS];
    Args args;
    args.size = 0;

    while (1) {
        read_input(input_buffer);
        parse_input(input_buffer, &args);
        printf("size %i\n", args.size);
        print_args(&args);
        /* free_args(*args); */
    }

    return 0;
}
