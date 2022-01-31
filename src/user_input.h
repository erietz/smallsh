#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "globals.h"

void read_input(char* input_buffer);
void input_to_args(char* input_buffer, Args* args);
void print_args(Args* args);

#endif // USER_INPUT_H
