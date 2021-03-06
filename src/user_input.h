#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "globals.h"
#include <stdio.h>

void read_input(char input_buffer[], int num_chars, FILE *input, BgProcess* bg_processes);
void input_to_args(char* input_buffer, RawArgs* args);
void args_to_command(RawArgs* args, Command* cmd);

#endif // USER_INPUT_H
