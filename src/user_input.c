/* headers */
#include "user_input.h"
#include "utility.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* macros */
#define PROMPT ": "     // input prompt on the command line

/* function definitions */

/*
*------------------------------------------------------------------------------
* Function: read_input()
*
* Description:
* read_input() is used for reading input from the user.
* 
* Function Arguments:
*     @param input_buffer: A char array to read raw text into from stdin
*     @param num_chars: The number of chars including the null terminator that
*     can be read into the input_buffer.
*     @param input: Should be stdin. This is left as a generic stream type for
*     testing purposes.
*     @param bg_processes: The global linked list of background process to kill
*     in case the user inputs ctrl-d (i.e. EOF) and wants to exit the shell.
*
* Return Value:
*     input_buffer will contain the contents read from the input.
*------------------------------------------------------------------------------
*/
void read_input(
    char input_buffer[],
    int num_chars,
    FILE *input,
    BgProcess* bg_processes
) {
    char *raw_input;

    memset(input_buffer, '\0', num_chars*sizeof(char));
    printf(PROMPT);
    fflush(stdout);
    raw_input = fgets(input_buffer, num_chars, input);

    if (raw_input == NULL) {
        // typing ctrl-d into a terminal sends EOF which causes fgets to return
        // null. In this case we can just exit the shell.
        exit_shell(bg_processes);
    }
}

/*
*------------------------------------------------------------------------------
* Function: input_to_args()
*
* Description:
* input_to_args() is used for spliting the raw input read by the user at each
* space into an array of known size.
* 
* Function Arguments:
*     @param input_buffer: A char array containing raw text
*     @param args: A struct to store the array and the size of the array
*
* Return Value:
*     args is modified in-place. If the input_buffer is empty or starts with a
*     comment character, then the size of args is set to 0.
*------------------------------------------------------------------------------
*/
void input_to_args(char input_buffer[], RawArgs* args) {
    char *token;
    args->size = 0;

    token = strtok(input_buffer, " \n");

    /* line is blank */
    if (token == NULL)
        return;

    /* line is a comment */
    if (strncmp(token, "#", 1) == 0)
        return;

    while (token != NULL) {
        strcpy(args->items[args->size], token);
        args->size += 1;
        token = strtok(NULL, " \n");
    }

}

/*
*------------------------------------------------------------------------------
* Function: args_to_command()
*
* Description:
* args_to_command() is used for parsing the args struct into a command struct
* that can be interpreted later by smallsh.
* 
* Function Arguments:
*     @param args: An initialized RawArgs struct.
*     @param cmd: A Command struct which will be initialized by the function
*
* Return Value:
*     TODO add this
*------------------------------------------------------------------------------
*/
void args_to_command(RawArgs* args, Command* cmd) {
    int cmd_argv_index = 0;
    cmd->argc = 0;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->bg = 0;
    char* arg_pid_loc;
    char tmp_str[MAX_CHARS];

    // blank line
    if (args->size == 0)
        return;

    cmd->file = args->items[0];

    for (int i = 0; i < args->size; i++) {

        // Expand any $$ to process id of smallsh
        if ((arg_pid_loc = strstr(args->items[i], "$$")) != NULL) {
            pid_t pid = getpid();
            char pid_as_str[10];
            sprintf(pid_as_str, "%i", pid);
            replace_str(args->items[i], "$$", pid_as_str, tmp_str);
        }

        // Redirecting stdin
        if (strcmp(args->items[i], "<") == 0) {
            cmd->input = args->items[i + 1];
            i += 1; // skip the "<" character
            continue; // skip the input file name as an arg
        }

        // Redirecting stdout
        if (strcmp(args->items[i], ">") == 0) {
            cmd->output = args->items[i + 1];
            i += 1; // skip the ">" character
            continue; // skip the output file as an arg
        }

        // Run process in background
        if (strcmp(args->items[i], "&") == 0 && i == args->size - 1) {
            cmd->bg = 1;
            break;  // we are at the end of the command
        }

        // Argument is not a special character so copy it into cmd argv
        strcpy(cmd->argv[cmd_argv_index], args->items[i]);
        cmd->argc += 1;
        cmd_argv_index++;
    }

}

