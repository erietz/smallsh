#include "builtin.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void exit_shell() {
    puts("TODO: Kill all background processes before exiting");
    exit(1);
}

int cd(char *path) {
    char *dir;

    if (path == NULL) {
        dir = getenv("HOME");
    } else {
        dir = path;
    }

    if (chdir(dir) == -1) {
        puts("failed to cd");
        return 1;
    }

    return 0;
}
