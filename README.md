In this assignment you will write smallsh your own shell in C. smallsh will
implement a subset of features of well-known shells, such as bash. Your program
will

- [x] Provide a prompt for running commands
- [ ] Handle blank lines and comments, which are lines beginning with the
  # character
- [ ] Provide expansion for the variable $$
- [ ] Execute 3 commands exit, cd, and status via code built into the shell
- [ ] Execute other commands by creating new processes using a function from
  the exec family of functions
- [ ] Support input and output redirection
- [ ] Support running commands in foreground and background processes
- [ ] Implement custom handlers for 2 signals, SIGINT and SIGTSTP