In this assignment you will write smallsh your own shell in C. smallsh will
implement a subset of features of well-known shells, such as bash. Your program
will

- [x] Provide a prompt for running commands
- [x] Handle blank lines and comments, which are lines beginning with the
  # character
- [x] Provide expansion for the variable `$$`
- [x] Execute 3 commands exit, cd, and status via code built into the shell
- [ ] Execute other commands by creating new processes using a function from
  the exec family of functions
- [ ] Support input and output redirection
- [ ] Support running commands in foreground and background processes
    - [ ] Automatically redirect stdin and stdout to /dev/null
- [ ] Implement custom handlers for 2 signals, SIGINT and SIGTSTP
