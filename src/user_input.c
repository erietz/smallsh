/* headers */
#include "user_input.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* macros */
#define PROMPT ": "     // input prompt on the command line

/* types */
/* function declarations */
static void expand_pid(char* input, char* output, int offset);

/* global variables */
/* function definitions */
void read_input(char* input_buffer) {
    char *raw_input;

    memset(input_buffer, '\0', MAX_CHARS*sizeof(char));
    printf(PROMPT);
    fflush(stdout);
    raw_input = fgets(input_buffer, MAX_CHARS, stdin);

    if (raw_input == NULL) {
        // typing ctrl-d into a terminal sends EOF which causes fgets to return
        // null. In this case we can just exit the shell.
        exit_shell();
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

void args_to_command(RawArgs* args, Command* cmd) {
    int cmd_argv_index = 0;
    cmd->argc = 0;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->bg = 0;
    char arg_expanded_pid[MAX_CHARS];
    char* arg_pid_loc;

    if (args->size == 0)
        return;

    cmd->file = args->items[0];

    for (int i = 0; i < args->size; i++) {

        // Expand any $$ to process id of smallsh
        if ((arg_pid_loc = strstr(args->items[i], "$$")) != NULL) {
            int offset = arg_pid_loc - args->items[i]; // index of substring $$
            expand_pid(args->items[i], arg_expanded_pid, offset);
        }

        // Redirecting stdin
        if (strcmp(args->items[i], "<") == 0) {
            cmd->input = args->items[i + 1];
            i += 1; // skip the "<" character
            continue; // skip the input file name as an arg
        }

        // Redirecting stdout
        if (strcmp(args->items[i], ">") == 0) {
            cmd->output = args->items[i + 1];
            i += 1; // skip the ">" character
            continue; // skip the output file as an arg
        }

        // Run process in background
        if (strcmp(args->items[i], "&") == 0 && i == args->size - 1) {
            cmd->bg = 1;
            break;  // we are at the end of the command
        }

        // Argument is not a special character so copy it into cmd argv
        strcpy(cmd->argv[cmd_argv_index], args->items[i]);
        cmd->argc += 1;
        cmd_argv_index++;
    }

}

// TODO: this only expands one occurance of $$. Maybe make this recursive?
static void expand_pid(char* input, char* output, int offset) {
    pid_t pid = getpid();
    int length = strlen(input);
    char pid_as_str[10]; // max int size = 2147483647

    sprintf(pid_as_str, "%i", pid);
    int pid_length = strlen(pid_as_str);

    for (int i=0; i<offset; i++) {
        output[i] = input[i];
    }

    int j=0;
    for (int i=offset; i<offset + pid_length; i++) {
        output[i] = pid_as_str[j];
        j += 1;
    }

    j=0;
    for (int i=offset + pid_length; i<pid_length + length; i++){
        output[i] = input[offset + 2 + j];  // 2 is for $$
        j += 1;
    }

    strcpy(input, output);
}
