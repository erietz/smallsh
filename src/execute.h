#ifndef EXECUTE_H
#define EXECUTE_H

#include "user_input.h"

int dispatch_cmd(RawArgs* args);
int run_external_cmd(RawArgs* args);

#endif // EXECUTE_H
