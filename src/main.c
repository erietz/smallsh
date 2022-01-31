#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int MAX_CHARS = 2048;
static const int MAX_ARGS = 512;
static const char PROMPT[] = ": ";

typedef struct Args {
    int size;
    char *items[];
} Args;


void read_input(char* user_input) {
    memset(user_input, '\0', sizeof(&user_input));
    printf(PROMPT);
    fgets(user_input, MAX_CHARS, stdin);
}

Args* parse_input(char* user_input) {
    char *ptr, *token, *current_arg;
    Args *args = malloc(sizeof(Args));
    args->size = 0;

    token = strtok_r(user_input, " ", &ptr);
    current_arg = calloc(strlen(token) + 1, sizeof(char));
    strcpy(current_arg, token);
    args->items[args->size] = current_arg;
    args->size += 1;

    while (token != NULL) {
        token = strtok_r(NULL, " ", &ptr);
        if (token != NULL) {
            current_arg = calloc(strlen(token) + 1, sizeof(char));
            strcpy(current_arg, token);
            args->items[args->size] = current_arg;
            args->size += 1;
        }
    }

    // trim off trailing new line from last item in args array
    int len = strlen(current_arg);
    if (current_arg[len - 1] == '\n') {
        current_arg[len - 1] = '\0';
    }
    return args;
}

void print_args(Args* args) {
    for (int i = 0; i < args->size; i++) {
        puts(args->items[i]);
    }
}

int main(int argc, char *argv[]) {
    char user_input[MAX_CHARS];
    Args *args;

    while (1) {
        read_input(user_input);
        args = parse_input(user_input);
        print_args(args);
    }

    return 0;
}
