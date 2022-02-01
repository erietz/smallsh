#include "user_input.h"
#include "execute.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    char input_buffer[MAX_CHARS];

    // TODO: I am allocating 2*512*2024 bytes on the stack and reusing this
    // memory. I will need to use malloc to put this memory on the heap to
    // avoid stack overflow. However, malloc will be expensive to call for each
    // command.

    RawArgs args = {
        .size = 0,
        .items = {}
    };
    Command cmd = {
        .file = NULL,
        .argc = 0,
        .argv = {},
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

    while (1) {
        read_input(input_buffer);
        input_to_args(input_buffer, &args);
        args_to_command(&args, &cmd);
        /* print_args(&args); */
        dispatch_cmd(&cmd);
    }

    return 0;
}
