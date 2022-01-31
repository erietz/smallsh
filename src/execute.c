#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <string.h>

int dispatch_cmd(Args *args){
    char *cmd;

    if (args->size == 0)
        return 0;

    cmd = args->items[0];

    if (strcmp(cmd, "exit") == 0) {
        puts("TODO: exit");
        return 0;
    } else if (strcmp(cmd, "status") == 0) {
        puts("TODO: status");
        return 0;
    } else if (strcmp(cmd, "cd") == 0) {
        puts("TODO: cd to dir or home");
        return 0;
    }

    return 0;
}
