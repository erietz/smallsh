#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>     // exit
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid

int dispatch_cmd(Command* command){
    char *cmd;

    if (command->argc == 0)
        return 0;

    cmd = command->argv[0];

    if (strcmp(cmd, "exit") == 0) {
        puts("TODO: exit");
    } else if (strcmp(cmd, "status") == 0) {
        puts("TODO: status");
    } else if (strcmp(cmd, "cd") == 0) {
        if (command->argc == 1) {
            cd(NULL);
        } else {
            cd(command->argv[1]);
        }
    } else {
        run_external_cmd(command);
    }

    return 0;
}

static void exec_cmd(Command* command, char* argv[]) {
    int input_fd = -1;
    int output_fd = -1;

    if (command->input != NULL) {
        input_fd = open(command->input, O_RDONLY);
        if (input_fd == -1) {
            perror("open()");
            exit(1);
        }
        int result = dup2(input_fd, 0);
        if (result == -1) {
            perror("dup2()");
            exit(1);
        }

        fcntl(input_fd, F_SETFD, FD_CLOEXEC);
    }

    if (command->output != NULL) {
        output_fd = open(command->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open()");
            exit(1);
        }
        int result = dup2(output_fd, 1);
        if (result == -1) {
            perror("dup2()");
            exit(1);
        }

        fcntl(output_fd, F_SETFD, FD_CLOEXEC);
    }

    execvp(argv[0], argv);
}

int run_external_cmd(Command* command) {
    char *argv[command->argc + 1];
    int child_status;
    pid_t spawn_pid;

    for (int i = 0; i < command->argc; i++) {
        argv[i] = command->argv[i];
    }
    argv[command->argc] = NULL;

    spawn_pid = fork();
    switch (spawn_pid) {
        case -1:
            perror("fork()\n");
            return -1;
            break;
        case 0:
            // child process
            exec_cmd(command, argv);
            perror("smallsh");
            return 0;
            break;
        default:
            // parent process (spawn_pid is the process id of the child)
            spawn_pid = waitpid(spawn_pid, &child_status, 0);
            fflush(stdout);
            return 1;
            break;
    }
}
