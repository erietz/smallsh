#ifndef EXECUTE_H
#define EXECUTE_H

#include "user_input.h"

typedef struct BgProcess {
    int pid;
    struct BgProcess* next;
} BgProcess;

int dispatch_cmd(Command* cmd);
int run_external_cmd(Command* cmd);
void append_bg_node(BgProcess* node, int pid);
void free_process_list(BgProcess* node);

int cd(char* path);
void status();
void exit_shell();

#endif // EXECUTE_H
