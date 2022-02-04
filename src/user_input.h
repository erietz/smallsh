#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "globals.h"
#include <stdio.h>

void read_input(char* input_buffer, int num_chars, FILE *input);
void input_to_args(char* input_buffer, RawArgs* args);
void args_to_command(RawArgs* args, Command* cmd);
void expand_pid(char* input, char* output, int offset);

#endif // USER_INPUT_H
