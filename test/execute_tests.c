#include "unit_test.h"
#include "../src/execute.h"
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

// cd {{{
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
// }}}

void status_test() {
}

void dispatch_cmd_test() {
}

// BgProcess List Tests {{{

void append_bg_node_test() {
    BgProcess* head = create_bg_node(-1);
    append_bg_node(head, 2);
    append_bg_node(head, 3);
    append_bg_node(head, 4);

    assert_int_equal(head->pid, -1);
    assert_int_equal(head->next->pid, 2);
    assert_int_equal(head->next->next->pid, 3);
    assert_int_equal(head->next->next->next->pid, 4);

    free_process_list(head);
}

void remove_bg_node_test() {
    BgProcess* head = create_bg_node(-1);
    append_bg_node(head, 2);
    append_bg_node(head, 3);
    append_bg_node(head, 4);

    remove_bg_node(head, 2);
    assert_int_equal(head->pid, -1);
    assert_int_equal(head->next->pid, 3);
    assert_int_equal(head->next->next->pid, 4);
    if (head->next->next->next == NULL) {
        assert_int_equal(1, 1);
    } else {
        assert_int_equal(0, 1);
    }

    remove_bg_node(head, 4);
    assert_int_equal(head->pid, -1);
    assert_int_equal(head->next->pid, 3);
    if (head->next->next == NULL) {
        assert_int_equal(1, 1);
    } else {
        assert_int_equal(0, 1);
    }

    remove_bg_node(head, 3);
    assert_int_equal(head->pid, -1);
    if (head->next == NULL) {
        assert_int_equal(1, 1);
    } else {
        assert_int_equal(0, 1);
    }

    free_process_list(head);
}

// }}}

void run_execute_tests() {
    cd_test();
    status_test();

    append_bg_node_test();
    remove_bg_node_test();
}
