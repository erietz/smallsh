In this assignment you will write smallsh your own shell in C. smallsh will
implement a subset of features of well-known shells, such as bash. Your program
will

- [x] Provide a prompt for running commands
- [x] Handle blank lines and comments, which are lines beginning with the
  # character
- [x] Provide expansion for the variable `$$`
- [x] Execute 3 commands exit, cd, and status via code built into the shell
- [x] Execute other commands by creating new processes using a function from
  the exec family of functions
- [x] Support input and output redirection
- [x] Support running commands in foreground and background processes
    - [x] Automatically redirect stdin and stdout to /dev/null
- [x] Implement custom handlers for 2 signals, SIGINT and SIGTSTP

The general syntax of a command line is: 
`command [arg1 arg2 ...] [< input_file] [> output_file] [&]`

...where items in square brackets are optional.

- You can assume that a command is made up of words separated by spaces.
- The special symbols <, > and & are recognized, but they must be surrounded by
  spaces like other words.
- If the command is to be executed in the background, the last word must be &.
  If the & character appears anywhere else, just treat it as normal text.
- If standard input or output is to be redirected, the > or < words followed by
  a filename word must appear after all the arguments. Input redirection can
  appear before or after output redirection.
- Your shell does not need to support any quoting; so arguments with spaces
  inside them are not possible. We are also not implementing the pipe "|"
  operator.
- Your shell must support command lines with a maximum length of 2048
  characters, and a maximum of 512 arguments.
- You do not need to do any error checking on the syntax of the command line.

# FIXME

1. hitting ctrl-c should display "terminated by process"
    - this could be the last cmd exit status when typing status
2. hitting ctrl-z does not restore cmd access without pressing enter again
3. background processes are being killed by repeated ctrl-c
