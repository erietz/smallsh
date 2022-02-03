#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // exit
#include <fcntl.h>      // file control
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid


static void exec_cmd(Command *cmd);

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

static void exec_cmd(Command* cmd) {
    int input_fd = -1;
    int output_fd = -1;
    char *argv[cmd->argc + 1];

    // execvp requires NULL terminated array
    for (int i = 0; i < cmd->argc; i++) {
        argv[i] = cmd->argv[i];
    }
    argv[cmd->argc] = NULL;

    // Redirect stdin to input file
    if (cmd->input != NULL) {
        // open the file to for the process to read from
        input_fd = open(cmd->input, O_RDONLY);
        if (input_fd == -1) {
            perror("open()");
            exit(1);
        }

        // make stdin refer to input_fd
        int result = dup2(input_fd, 0);
        if (result == -1) {
            perror("dup2()");
            exit(1);
        }

        // close the file descriptor after exec function finishes
        fcntl(input_fd, F_SETFD, FD_CLOEXEC);
    }

    // Redirect stdout to output file
    if (cmd->output != NULL) {
        output_fd = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open()");
            exit(1);
        }

        // make stdout refer to output_fd
        int result = dup2(output_fd, 1);
        if (result == -1) {
            perror("dup2()");
            exit(1);
        }

        // close the file descriptor after exec function finishes
        fcntl(output_fd, F_SETFD, FD_CLOEXEC);
    }

    // execute the command from the argument vector from files in PATH
    execvp(argv[0], argv);
}

int run_external_cmd(Command* cmd) {
    int child_status;
    pid_t spawn_pid;

    spawn_pid = fork();
    switch (spawn_pid) {
        case -1:
            perror("fork()\n");
            return -1;
            break;
        case 0:
            // child process
            exec_cmd(cmd);
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
