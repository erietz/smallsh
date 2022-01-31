#ifndef USER_INPUT_H
#define USER_INPUT_H

/* maximum number of chacters that can be read from user*/
#define MAX_CHARS 2048
/* maximum number of space delimited words that can be read from user*/
#define MAX_ARGS 512
/* input prompt on the command line */
#define PROMPT ": "

typedef struct Args {
    int size;
    char items[MAX_ARGS][MAX_CHARS];
} Args;

void read_input(char* input_buffer);
void parse_input(char* input_buffer, Args* args);
void print_args(Args* args);

#endif // USER_INPUT_H
