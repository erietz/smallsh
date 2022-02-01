#ifndef EXECUTE_H
#define EXECUTE_H

#include "user_input.h"

int dispatch_cmd(Command* cmd);
int run_external_cmd(Command* cmd);

#endif // EXECUTE_H
