#ifndef GLOBALS_H
#define GLOBALS_H

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

#endif // GLOBALS_H
