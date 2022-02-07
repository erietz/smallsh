/* headers */
#include "execute.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>      // file control
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid
#include <signal.h>     // handling ctrl-c and ctrl-z


/*
* TODO:
*   - not printing signal information if background process killed by signal
*   - background processes are being killed by repeated ctrl-c
*   - add docstrings and clean up
*/

/* macros */
/* types */
/* function declarations */
static void exec_cmd(Command *cmd);
static void handle_SIGTSTP(int sig_num);
static void print_signal_status(int pid, int status);

/* global variables */
int foreground_only_mode = 0;
int last_cmd_exit_status = 0;
struct sigaction sa_sigint = {{0}};
struct sigaction sa_sigtstp = {{0}};


/* function definitions */
// Executing commands {{{

void dispatch_cmd(Command* command, BgProcess* bg_processes){
    if (command->argc == 0)
        return;

    char *cmd = command->argv[0];

    if (strcmp(cmd, "exit") == 0) {
        exit_shell(bg_processes);
    } else if (strcmp(cmd, "status") == 0) {
        status();
    } else if (strcmp(cmd, "cd") == 0) {
        if (command->argc == 1) {
            cd(NULL);
        } else {
            cd(command->argv[1]);
        }
    } else {
        run_external_cmd(command, bg_processes);
    }

}

void run_external_cmd(Command* cmd, BgProcess* bg_processes) {
    int child_status;
    pid_t spawn_pid;

    spawn_pid = fork();
    switch (spawn_pid) {
        case -1:
            perror("fork()\n");
            fflush(stdout);
            exit(1);
            break;
        case 0:
            // child process

            // ignore ctrl-z
            sa_sigtstp.sa_handler = SIG_IGN;
            sigaction(SIGTSTP, &sa_sigtstp, NULL);

            if (cmd->bg == 0) {
                // make ctrl-c interupt the child process
                sa_sigint.sa_handler = SIG_DFL;     // default handling of ctrl-c
                sigaction(SIGINT, &sa_sigint, NULL);    // install handler
            }

            if (cmd->bg == 1 && cmd->input == NULL && foreground_only_mode == 0) {
                cmd->input = "/dev/null";
            }

            if (cmd->bg == 1 && cmd->output == NULL && foreground_only_mode == 0) {
                cmd->output = "/dev/null";
            }

            exec_cmd(cmd);  // function only returns if error rwith exec

            perror("smallsh");
            fflush(stdout);
            exit(1);
            break;
        default:
            // parent process (default = spawn_pid = the process id of the
            // child)

            if (cmd->bg == 1 && foreground_only_mode == 0) {
                // Execute in the background
                waitpid(spawn_pid, &child_status, WNOHANG);
                printf("Running %s in background with pid %d\n", cmd->argv[0], spawn_pid);
                fflush(stdout);
                append_bg_node(bg_processes, spawn_pid);
            } else {
                // Execute and block in the foreground
                waitpid(spawn_pid, &child_status, 0);
                print_signal_status(spawn_pid, child_status);
                last_cmd_exit_status = WEXITSTATUS(child_status);
            }
            break;
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
        // open the file which the process will read from
        input_fd = open(cmd->input, O_RDONLY);
        if (input_fd == -1) {
            printf("cannot open %s for input\n", cmd->input);
            fflush(stdout);
            exit(1);
        }

        // make stdin refer to input_fd
        int result = dup2(input_fd, STDIN_FILENO);
        if (result == -1) {
            perror("dup2()");
            fflush(stdout);
            exit(1);
        }

        // close the input file descriptor after exec function finishes
        fcntl(input_fd, F_SETFD, FD_CLOEXEC);
    }

    // Redirect stdout to output file
    if (cmd->output != NULL) {
        output_fd = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644); // TODO is this the correct permission
        if (output_fd == -1) {
            printf("cannot open %s for output\n", cmd->output);
            fflush(stdout);
            exit(1);
        }

        // make stdout refer to output_fd
        int result = dup2(output_fd, STDOUT_FILENO);
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

BgProcess* create_bg_node(int pid) {
    BgProcess* node = malloc(sizeof(BgProcess));
    node->pid = pid;
    node->next = NULL;
    return node;
}

void append_bg_node(BgProcess* node, int pid) {
    BgProcess* new_node = create_bg_node(pid);

    while (node->next != NULL) {
        node = node->next;
    }

    node->next = new_node;
}

void remove_bg_node(BgProcess* head, int pid) {
    BgProcess* curr = head;
    BgProcess* prev;

    while (curr != NULL) {
        if (curr->pid == pid) {
            prev->next = curr->next;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

void free_process_list(BgProcess* node){
    BgProcess* tmp;

    while (node != NULL) {
        tmp = node;
        node = node->next;
        free(tmp);
    }
}

void cleanup_bg_processes(BgProcess* bg_processes) {
    BgProcess* curr = bg_processes;
    BgProcess* tmp;
    int status;

    // loop through each bg_process and remove node if process completed
    while (curr != NULL) {
        // skip head of list with phony pid
        if (curr->pid == -1) {
            curr = curr->next;
            continue;
        }

        // save curr->next as the loop counter as curr may be removed
        tmp = curr->next;

        // waitpid returns 0 if the process is not completed
        int pid = waitpid(curr->pid, &status, WNOHANG);

        if (pid > 0) {
            printf(
                "Process %d completed. Exit status: %d\n",
                curr->pid,
                WEXITSTATUS(status)
            );
            fflush(stdout);
            remove_bg_node(bg_processes, curr->pid);
        }
        curr = tmp;

        /* if (WIFSIGNALED(status)) { */
        /*     printf("%d killed by signal %d %s\n", */
        /*         curr->pid, */
        /*         WTERMSIG(status), */
        /*         strsignal(WTERMSIG(status)) */
        /*      ); */
        /*     remove_bg_node(&bg_processes, curr->pid); */
        /* } else if (WIFEXITED(status)) { */
        /*     printf("%d exited: status = %d\n", curr->pid, WEXITSTATUS(status)); */
        /*     remove_bg_node(&bg_processes, curr->pid); */
        /* } else if (WIFSTOPPED(status)) { */
        /*     printf("%d stopped by signal %d %s\n", */
        /*         curr->pid, */
        /*         WSTOPSIG(status), */
        /*         strsignal(WSTOPSIG(status)) */
        /*     ); */
        /* } */

    }
}

// }}}
// builtin commands {{{
void exit_shell(BgProcess* bg_processes) {
    BgProcess *curr = bg_processes;

    // no background processes have been added

    while (curr != NULL) {
        if (curr->pid == -1) {
            curr = curr->next;
            continue;
        }

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
    /*
    * Parent process and background child processes ignore SIGINT. Only a child
    * process executing in the foreground will respond to SIGINT. This signal
    * is turned on (elsewhere) in the child process.
    */

    // handle ctrl-c------------------------------------------------------------
    sa_sigint.sa_handler = SIG_IGN;     // ignore ctrl-c
    sa_sigint.sa_flags = 0;

    // block all signals while sa_handler is running
    sigfillset(&sa_sigint.sa_mask);

    // install the handler
    sigaction(SIGINT, &sa_sigint, NULL);


    // handle ctrl-z------------------------------------------------------------
    sa_sigtstp.sa_handler = handle_SIGTSTP;

    // automatic restart of the interrupted system call or library function
    // after the signal handler gets done.
    sa_sigtstp.sa_flags = SA_RESTART;

    // block all signals while sa_handler is running
    sigfillset(&sa_sigtstp.sa_mask);

    // install the handler
    sigaction(SIGTSTP, &sa_sigtstp, NULL);
}

// Custom handler for CTRL-Z
static void handle_SIGTSTP(int sig_num) {
    if (foreground_only_mode == 0) {
        char *message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 50);
        fflush(stdout);
        foreground_only_mode = 1;
    } else {
        char *message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 30);
        fflush(stdout);
        foreground_only_mode = 0;
    }
}

static void print_signal_status(int pid, int status) {
    if (WIFSIGNALED(status)) {
        printf("%d killed by signal %d %s\n",
            pid,
            WTERMSIG(status),
            strsignal(WTERMSIG(status))
         );
    }
}

// }}}
