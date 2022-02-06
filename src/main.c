/* headers */
#include "user_input.h"
#include "globals.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>

/* global variables */
char input_buffer[MAX_CHARS];
RawArgs args = {0};
Command cmd = {0};
BgProcess* bg_processes;

/* function definitions */
int main(int argc, char *argv[]) {
    bg_processes = create_bg_node(-1);

    initialize_signal_handlers();

    while (1) {
        read_input(input_buffer, MAX_CHARS, stdin, bg_processes);
        input_to_args(input_buffer, &args);
        args_to_command(&args, &cmd);
        dispatch_cmd(&cmd, bg_processes);
        watch_bg_processes(bg_processes);
    }

    return 0;
}
