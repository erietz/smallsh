/* headers */
#include "execute.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // exit
#include <fcntl.h>      // file control
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid
#include <signal.h>


/* macros */
/* types */
/* function declarations */
static void exec_cmd(Command *cmd);
/* static void handle_SIGINT(int sig_num); */
static void handle_SIGTSTP(int sig_num);

/* global variables */
int current_process_in_background = 0;
int last_cmd_exit_status = 0;
BgProcess bg_processes = { .pid = -1, .next = NULL, };
struct sigaction sa_sigint = {{0}};
struct sigaction sa_sigtstp = {{0}};


/* function definitions */
// Executing commands {{{

void dispatch_cmd(Command* command){
    if (command->argc == 0)
        return;

    char *cmd = command->argv[0];

    if (strcmp(cmd, "exit") == 0) {
        exit_shell();
    } else if (strcmp(cmd, "status") == 0) {
        status();
    } else if (strcmp(cmd, "cd") == 0) {
        if (command->argc == 1) {
            cd(NULL);
        } else {
            cd(command->argv[1]);
        }
    } else {
        run_external_cmd(command);
    }

}

int run_external_cmd(Command* cmd) {
    int child_status;
    pid_t spawn_pid;

    spawn_pid = fork();
    switch (spawn_pid) {
        case -1:
            perror("fork()\n");
            fflush(stdout);
            return 2;
            break;
        case 0:
            // child process

            sa_sigint.sa_handler = SIG_DFL;     // default handling of ctrl-c
            sigaction(SIGINT, &sa_sigint, NULL);    // install handler

            exec_cmd(cmd);  // function only returns if error rwith exec
            perror("smallsh");
            fflush(stdout);
            return 2;
            break;
        default:
            // parent process (default = spawn_pid = the process id of the
            // child)

            if (cmd->bg == 1 && current_process_in_background == 0) {
                // Execute in the background
                waitpid(spawn_pid, &child_status, WNOHANG);
                printf("Background pid: %d\n", spawn_pid);
                fflush(stdout);
                append_bg_node(&bg_processes, spawn_pid);
            } else {
                // Execute and block in the foreground
                waitpid(spawn_pid, &child_status, 0);
                last_cmd_exit_status = child_status;
            }
            fflush(stdout);

            return child_status;
            break;

            /*
            Consider storing the PIDs of non-completed background processes in
            an array. Then every time BEFORE returning access to the command
            line to the user, you can check the status of these processes using
            waitpid(...NOHANG...). 

            Alternatively, you may use a signal handler to immediately wait()
            for child processes that terminate, as opposed to periodically
            checking a list of started background processes

        */
    }
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
            fflush(stdout);
            exit(1);
        }

        // make stdin refer to input_fd
        int result = dup2(input_fd, 0);
        if (result == -1) {
            perror("dup2()");
            fflush(stdout);
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
            fflush(stdout);
            exit(1);
        }

        // make stdout refer to output_fd
        int result = dup2(output_fd, 1);
        if (result == -1) {
            perror("dup2()");
            fflush(stdout);
            exit(1);
        }

        // close the file descriptor after exec function finishes
        fcntl(output_fd, F_SETFD, FD_CLOEXEC);
    }

    // execute the command from the argument vector from files in PATH
    execvp(argv[0], argv);
}

// }}}
// Managing background processes {{{

void append_bg_node(BgProcess* node, int pid) {
    if (node->pid == -1) {
        node->pid = pid;
        return;
    }

    BgProcess* new_node = malloc(sizeof(BgProcess));
    new_node->pid = pid;
    new_node->next = NULL;

    while (node->next != NULL) {
        node = node->next;
    }

    node->next = new_node;
}

void free_process_list(BgProcess* node){
    BgProcess* tmp;

    while (node != NULL) {
        tmp = node;
        node = node->next;
        free(tmp);
    }
}

// }}}
// builtin commands {{{
void exit_shell() {
    BgProcess *curr = &bg_processes;

    // no background processes have been added
    if (curr->pid == -1)
        exit(0);

    while (curr != NULL) {
        // TODO: delete this line
        printf("killing process %i\n", curr->pid);
        fflush(stdout);
        kill(curr->pid, SIGKILL);
        curr = curr->next;
    }

    exit(0);
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

void status() {
    printf("Last command exit status %d\n", last_cmd_exit_status);
    fflush(stdout);
}

// }}}
// Signal Handlers {{{

void initialize_signal_handlers() {
    /* sa_sigint.sa_handler = handle_SIGINT; */
    sa_sigint.sa_handler = SIG_IGN;     // ignore ctrl-c
    sa_sigint.sa_flags = 0;
    // block all signals while sa_handler is running
    sigfillset(&sa_sigint.sa_mask);
    // install the handler
    sigaction(SIGINT, &sa_sigint, NULL);

    sa_sigtstp.sa_handler = handle_SIGTSTP;
    sa_sigtstp.sa_flags = 0;
    // block all signals while sa_handler is running
    sigfillset(&sa_sigtstp.sa_mask);
    // install the handler
    sigaction(SIGTSTP, &sa_sigtstp, NULL);
}

/* // Handler for CTRL-C */
/* static void handle_SIGINT(int sig_num) { */
/* } */

// Handler for CTRL-Z
static void handle_SIGTSTP(int sig_num) {
    // TODO: this breaks if pressing ctrl-z more than once since it puts
    // smallsh itself in the background
    if (current_process_in_background == 0) {
        char *message = "Suspending current job\n";
        write(STDOUT_FILENO, message, 24);
        current_process_in_background = 1;
    } else {
        char *message = "Suspended jobs, cannot put process in the background\n";
        write(STDOUT_FILENO, message, 54);
    }
}

// }}}
