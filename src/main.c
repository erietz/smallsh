#include "user_input.h"
#include "execute.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    char input_buffer[MAX_CHARS];
    Args args = {
        .size = 0,
        .items = {}
    };

    while (1) {
        read_input(input_buffer);
        input_to_args(input_buffer, &args);
        /* print_args(&args); */
        dispatch_cmd(&args);
    }

    return 0;
}
