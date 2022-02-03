#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "globals.h"

void read_input(char* input_buffer);
void input_to_args(char* input_buffer, RawArgs* args);
void args_to_command(RawArgs* args, Command* cmd, int pid);

#endif // USER_INPUT_H
