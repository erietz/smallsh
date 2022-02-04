/* headers */
#include "user_input.h"
#include "execute.h"
#include <stdio.h>

/* macros */
/* types */
/* function declarations */
/* global variables */
char input_buffer[MAX_CHARS];
RawArgs args = {0};
Command cmd = {0};

/* function definitions */
int main(int argc, char *argv[]) {

    initialize_signal_handlers();

    while (1) {
        read_input(input_buffer, MAX_CHARS, stdin);
        input_to_args(input_buffer, &args);
        args_to_command(&args, &cmd);
        dispatch_cmd(&cmd);
    }

    return 0;
}
