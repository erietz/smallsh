#include "user_input.h"
#include "execute.h"
#include <stdio.h>
#include <unistd.h>

char input_buffer[MAX_CHARS];

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

int main(int argc, char *argv[]) {
    pid_t pid = getpid();

    while (1) {
        read_input(input_buffer);
        input_to_args(input_buffer, &args);
        args_to_command(&args, &cmd, pid);
        dispatch_cmd(&cmd);
    }

    return 0;
}
