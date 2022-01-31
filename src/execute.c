#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // exit
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid

int dispatch_cmd(Args *args){
    char *cmd;

    if (args->size == 0)
        return 0;

    cmd = args->items[0];

    if (strcmp(cmd, "exit") == 0) {
        puts("TODO: exit");
    } else if (strcmp(cmd, "status") == 0) {
        puts("TODO: status");
    } else if (strcmp(cmd, "cd") == 0) {
        if (args->size == 1) {
            cd(NULL);
        } else {
            cd(args->items[1]);
        }
    } else {
        run_external_cmd(args);
    }

    return 0;
}

int run_external_cmd(Args* args) {
    char *argv[args->size + 1];
    int child_status;
    pid_t spawn_pid;

    for (int i = 0; i < args->size; i++) {
        argv[i] = args->items[i];
    }
    argv[args->size] = NULL;

    spawn_pid = fork();

    switch (spawn_pid) {
        case -1:
            perror("fork()\n");
            return -1;
            break;
        case 0:
            // child process
            execvp(argv[0], argv);
            perror("smallsh");
            return 0;
            break;
        default:
            // parent process (spawn_pid is the process id of the child)
            spawn_pid = waitpid(spawn_pid, &child_status, 0);
            return 1;
            break;
    }
}
