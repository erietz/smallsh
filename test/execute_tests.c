#include "unit_test.h"
#include "../src/execute.h"
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

void cd_test() {
    // ARRANGE
    char *home = getenv("HOME");
    char wd_save[PATH_MAX];
    char wd_tmp[PATH_MAX];
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    strcpy(wd_save, cwd);
    strcpy(wd_tmp, cwd);

    // ACT
    cd("test");
    getcwd(cwd, sizeof(cwd));

    // ASSERT
    strcat(wd_tmp, "/test");
    assert_str_equal(cwd, wd_tmp);

    // ACT
    cd(NULL);
    getcwd(cwd, sizeof(cwd));

    // ASSERT
    assert_str_equal(cwd, home);

    cd("/usr/bin");
    getcwd(cwd, sizeof(cwd));
    assert_str_equal(cwd, "/usr/bin");

}

void status_test() {
}

void dispatch_cmd_test() {
}


void run_execute_tests() {
    cd_test();
    status_test();
}
