#ifndef GLOBALS_H
#define GLOBALS_H

/* maximum number of chacters that can be read from user*/
#define MAX_CHARS 2048
/* maximum number of space delimited words that can be read from user*/
#define MAX_ARGS 512
/* input prompt on the command line */
#define PROMPT ": "

typedef struct RawArgs {
    int size;
    char items[MAX_ARGS][MAX_CHARS];
} RawArgs;

typedef struct Command {
    char *file;
    char args[MAX_ARGS][MAX_CHARS];
    char *input;
    char *output;
    int bg;
} Command;

#endif // GLOBALS_H
