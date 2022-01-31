#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 2048
#define MAX_ARGS 512
#define PROMPT ": "

typedef struct Args {
    int size;
    char *items[MAX_ARGS];
} Args;


void read_input(char* user_input) {
    char *tmp;

    memset(user_input, '\0', MAX_CHARS*sizeof(char));
    printf(PROMPT);
    tmp = fgets(user_input, MAX_CHARS, stdin);    // TODO: cannot read passed \n char
    if (tmp == NULL) {
        puts("something bad happened");
    }
}

Args* parse_input(char* user_input) {
    char *token, *current_arg;
    Args *args = malloc(sizeof(Args));
    args->size = 0;
    args->items[0] = "";

    token = strtok(user_input, " \n");
    current_arg = strdup(token);
    args->items[args->size] = current_arg;
    args->size += 1;

    while (token != NULL) {
        token = strtok(NULL, " \n");
        if (token != NULL) {
            current_arg = strdup(token);
            args->items[args->size] = current_arg;
            args->size += 1;
        }
    }

    /* TODO: If using fgets this is unnessary since I can use " \n" in strtok */
    /* // trim off trailing new line from last item in args array */
    /* int len = strlen(current_arg); */
    /* if (current_arg[len - 1] == '\n') { */
    /*     current_arg[len - 1] = '\0'; */
    /* } */

    return args;
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
    free(args);
}

int main(int argc, char *argv[]) {
    char user_input[MAX_CHARS];
    Args *args;

    while (1) {
        read_input(user_input);
        args = parse_input(user_input);
        printf("size %i\n", args->size);
        print_args(args);
        free_args(args);
    }

    return 0;
}
