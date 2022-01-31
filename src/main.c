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
    raw_input = fgets(input_buffer, MAX_CHARS, stdin);

    if (raw_input == NULL) {
        puts("Error with fgets or EOF");
    }
}

void parse_input(char* input_buffer, Args* args) {
    char *token;
    args->size = 0;

    token = strtok(input_buffer, " \n");
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
    for (int i = 0; i < args->size; i++) {
        puts(args->items[i]);
    }
}

int main(int argc, char *argv[]) {
    char input_buffer[MAX_CHARS];
    Args args = {
        .size = 0,
        .items = {}
    };

    while (1) {
        read_input(input_buffer);
        parse_input(input_buffer, &args);
        printf("size %i\n", args.size);
        print_args(&args);
    }

    return 0;
}
