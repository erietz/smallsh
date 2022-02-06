#ifndef EXECUTE_H
#define EXECUTE_H

#include "user_input.h"
#include "globals.h"

extern BgProcess* bg_processes;

void dispatch_cmd(Command* cmd);
int run_external_cmd(Command* cmd);
void append_bg_node(BgProcess* node, int pid);
void free_process_list(BgProcess* node);
void initialize_signal_handlers();
void watch_bg_processes();

int cd(char* path);
void status();
void exit_shell();

#endif // EXECUTE_H
