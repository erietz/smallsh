/* headers */
#include "execute.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>      // file control
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // execvp, fork, getpid
#include <signal.h>     // handling ctrl-c and ctrl-z


/* function declarations */
static void exec_cmd(Command *cmd);
static void handle_SIGTSTP(int sig_num);


/* global variables */
int foreground_only_mode = 0;
int last_cmd_status = 0;
struct sigaction sa_sigint = {{0}};
struct sigaction sa_sigtstp = {{0}};


/* function definitions */

// Executing commands {{{
/*
*------------------------------------------------------------------------------
* Function: dispatch_cmd()
*
* Description:
* dispatch_cmd() is used for taking an initialized Command struct and sending
* off the command to be executed elsewhere (i.e. via a built-in command or via
* a child process).
* 
* Function Arguments:
*     @param command: An Command struct which has been initialized by the
*     args_to_command function.
*     @param bg_processes: The linked list of background process running in the
*     shell.
*------------------------------------------------------------------------------
*/
void dispatch_cmd(Command* command, BgProcess* bg_processes){
    if (command->argc == 0)
        return;

    char *cmd = command->argv[0];

    if (strcmp(cmd, "exit") == 0) {
        exit_shell(bg_processes);
    } else if (strcmp(cmd, "status") == 0) {
        print_status(last_cmd_status);
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

/*
*------------------------------------------------------------------------------
* Function: run_external_cmd()
*
* Description:
* run_external_cmd() is used for running any commands that are not built-in
* commands as a child process. NOTE: If a command is to be executed as a
* background process, and the input and/or output members of the struct are not
* set for redirection, they will be automatically redirected to /dev/null.
*
* Function Arguments:
*     @param cmd: An initialized Command obtained from the args_to_command
*     function.
*     @param bg_processes: The linked list of background processes being run in
*     the shell.
*
* Return Value:
*     The global variable last_cmd_status is updated for any process that
*     gets executed in the foreground.
*------------------------------------------------------------------------------
*/
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
            sa_sigtstp.sa_flags = 0;
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

            perror(cmd->argv[0]);
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
                last_cmd_status = child_status;
                if (WIFSIGNALED(child_status)) {
                    printf("process %d terminated by signal %d\n",
                        spawn_pid, child_status
                    );
                }
            }
            break;
    }
}

/*
*------------------------------------------------------------------------------
* Function: exec_cmd()
*
* Description:
* exec_cmd() is used for executing a given command in the current process.
* 
* Function Arguments:
*     @param cmd: An initialized Command struct.
*
* Return Value:
*     The function will only return if the command does not exit successfully.
*------------------------------------------------------------------------------
*/
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

/*
*------------------------------------------------------------------------------
* Function: create_bg_node()
*
* Description:
* create_bg_node() is used for creating a new dynamically allocated background
* node to be used in a linked list. The node will need to be freed. NOTE: The
* head of the linked list must always have a phony pid of -1.
* 
* Function Arguments:
*     @param pid: The process id of the background process
*
* Return Value:
*     @return BgProcess: A pointer to a BgProcess struct node.
*------------------------------------------------------------------------------
*/
BgProcess* create_bg_node(int pid) {
    BgProcess* node = malloc(sizeof(BgProcess));
    node->pid = pid;
    node->next = NULL;
    return node;
}

/*
*------------------------------------------------------------------------------
* Function: append_bg_node()
*
* Description:
* append_bg_node() is used for creating a new background process node and
* adding it the then end of a linked list. NOTE: The head of the linked list
* must always have a phony pid of -1.

*
* Function Arguments:
*     @param node: The head of the linked list
*     @param pid: The process id of the node that will be appended.
*------------------------------------------------------------------------------
*/
void append_bg_node(BgProcess* node, int pid) {
    BgProcess* new_node = create_bg_node(pid);

    while (node->next != NULL) {
        node = node->next;
    }

    node->next = new_node;
}

/*
*------------------------------------------------------------------------------
* Function: remove_bg_node()
*
* Description:
* remove_bg_node() is used for removing and freeing a node in a linked list of
* background processes. NOTE: The head of the linked list must always have a
* phony pid of -1.
*
* Function Arguments:
*     @param head: The head of the linked list
*     @param pid: The process id of the node to be removed.
*------------------------------------------------------------------------------
*/
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

/*
*------------------------------------------------------------------------------
* Function: free_process_list()
*
* Description:
* free_process_list() is used for completely freeing a linked list.
* 
* Function Arguments:
*     @param node: The head of the linked list to be freed.
*------------------------------------------------------------------------------
*/
void free_process_list(BgProcess* node){
    BgProcess* tmp;

    while (node != NULL) {
        tmp = node;
        node = node->next;
        free(tmp);
    }
}

/*
*------------------------------------------------------------------------------
* Function: cleanup_bg_processes()
*
* Description:
* cleanup_bg_processes() is used for watching each process in the linked list
* of background processes, printing a message about the exit status of the
* process, and removing them if they have completed.
* 
* Function Arguments:
*     @param bg_processes: The head of the linked list of background processes.
*------------------------------------------------------------------------------
*/
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
            printf("background pid %d is done: ", curr->pid);
            print_status(status);
            fflush(stdout);
            remove_bg_node(bg_processes, curr->pid);
        }
        curr = tmp;

    }
}

// }}}
// builtin commands {{{

/*
*------------------------------------------------------------------------------
* Function: exit_shell()
*
* Description:
* exit_shell() is used for exiting the shell.
* 
* Function Arguments:
*     @param bg_processes: The linked list of background processes to kill
*     before exiting.
*------------------------------------------------------------------------------
*/
void exit_shell(BgProcess* bg_processes) {
    BgProcess *curr = bg_processes;

    while (curr != NULL) {
        if (curr->pid == -1) {
            curr = curr->next;
            continue;
        }

        fflush(stdout);
        kill(curr->pid, SIGKILL);
        curr = curr->next;
    }

    exit(0);
}

/*
*------------------------------------------------------------------------------
* Function: cd()
*
* Description:
* cd() is used for changing the working directory of the shell to the provided
* path.
* 
* Function Arguments:
*     @param path: A relative or absolute path of the destination directory. If
*     path is NULL, this function attempts to change directory to $HOME.
*
* Return Value:
*     @return bool: 1 if the working directory is changed, else 0.
*------------------------------------------------------------------------------
*/
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

/*
*------------------------------------------------------------------------------
* Function: print_status()
*
* Description:
* print_status() is used for printing out a message regarding a status of a
* process. That is, whether the process exited properly or was terminated by a
* signal.
*
* Function Arguments:
*   @param status: An integer returned by wait() or waitpid()
*------------------------------------------------------------------------------
*/
void print_status(int status) {
    if (WIFSIGNALED(status)) {
        printf("terminated by signal %d.\n",
            WTERMSIG(status)
         );
    } else if (WIFEXITED(status)) {
        printf("exit value %d\n",
            WEXITSTATUS(status)
        );
    }

    fflush(stdout);
}

// }}}
// Signal Handlers {{{

/*
*------------------------------------------------------------------------------
* Function: initialize_signal_handlers()
*
* Description:
* initialize_signal_handlers() is used for setting up the default action for
* signal handlers used by the shell. Default action for smallsh means:
*
* CTRL-C (AKA SIGINT)
* - The parent process and any processes running in the background ignore
*   SIGINT
* - A child process running in the foreground terminates itself upon receiving
*   SIGINT.
*
* CTRL-Z (AKA SIGTSTP)
* - A child process running in the foreground ignores SIGTSTP.
* - A child process running in the background ignores SIGTSTP.
* - The parent process running the shell toggles between normal mode and
*   "foreground only mode" whereby new processes can no longer be run in the
*   background.
*------------------------------------------------------------------------------
*/
void initialize_signal_handlers() {
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

/*
*------------------------------------------------------------------------------
* Function: handle_SIGTSTP()
*
* Description:
* handle_SIGTSTP() is used as a custom handler by the parent process for the
* SIGTSTP signal. The child process must remove this custom handler.
*
* Function Arguments:
*     @param sig_num: The integer signal number that is being handled. Since
*     this function as passed as a pointer to another function, sig_int is not
*     literally passed.
*
* Return Value:
*     The global boolean variable foreground_only_mode is toggled.
*------------------------------------------------------------------------------
*/
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

// }}}
