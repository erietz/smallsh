/* headers */
#include "user_input.h"
#include "globals.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>

/* macros */
/* types */
/* function declarations */
/* global variables */
char input_buffer[MAX_CHARS];
RawArgs args = {0};
Command cmd = {0};
BgProcess* bg_processes;

/* function definitions */
int main(int argc, char *argv[]) {
    bg_processes = malloc(sizeof(BgProcess));
    bg_processes->pid = -1;
    bg_processes->next = NULL;

    initialize_signal_handlers();

    while (1) {
        watch_bg_processes();
        read_input(input_buffer, MAX_CHARS, stdin);
        input_to_args(input_buffer, &args);
        args_to_command(&args, &cmd);
        dispatch_cmd(&cmd);
    }

    return 0;
}
