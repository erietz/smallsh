#ifndef EXECUTE_H
#define EXECUTE_H

#include "user_input.h"
#include "globals.h"

void dispatch_cmd(Command* cmd, BgProcess* head);
void run_external_cmd(Command* cmd, BgProcess* head);
BgProcess* create_bg_node(int pid);
void append_bg_node(BgProcess* node, int pid);
void remove_bg_node(BgProcess* head, int pid);
void free_process_list(BgProcess* node);
void initialize_signal_handlers();
void cleanup_bg_processes(BgProcess* head);

int cd(char* path);
void status();
void exit_shell(BgProcess* bg_processes);

#endif // EXECUTE_H
