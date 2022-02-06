#include "../src/user_input.h"
#include "../src/globals.h"
#include "../src/utility.h"
#include "../src/execute.h"
#include "unit_test.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

// read_input_test {{{

void read_input_test() {
    char* input = "ls -al\n";
    FILE *stream = fmemopen(input, strlen(input), "r");
    char input_buffer[MAX_CHARS];
    BgProcess* bg_processes = create_bg_node(-1);

    read_input(input_buffer, MAX_CHARS, stream, bg_processes);

    assert_str_equal(input_buffer, input);
}

// }}}
// input_to_args_test {{{

void input_to_args_test_blank_line() {
    char input[] = "\n";
    RawArgs args = {0};

    input_to_args(input, &args);

    assert_int_equal(args.size, 0);
}

void input_to_args_test_comment_line() {
    char input[] = "# ls -al\n";
    RawArgs args = {0};

    input_to_args(input, &args);

    assert_int_equal(args.size, 0);
}

void input_to_args_test_real_input() {
    char input[] = "ls -al";
    RawArgs args = {0};

    input_to_args(input, &args);

    assert_int_equal(args.size, 2);
    assert_str_equal(args.items[0], "ls");
    assert_str_equal(args.items[1], "-al");
}

// }}}
// replace_str_test {{{

void replace_str_test() {
    char input[50] = "foo bar baz";
    char *old = "foo";
    char *new = "qux";
    char tmp[50];

    replace_str(input, old, new, tmp);
    assert_str_equal(input, "qux bar baz");

    replace_str(input, "baz", "spaz", tmp);
    assert_str_equal(input, "qux bar spaz");

    replace_str(input, "bar", "waldo", tmp);
    assert_str_equal(input, "qux waldo spaz");

    replace_str(input, "qux", "waldo", tmp);
    assert_str_equal(input, "waldo waldo spaz");

    replace_str(input, "waldo", "fred", tmp);
    assert_str_equal(input, "fred fred spaz");

    replace_str(input, "waldo", "fred", tmp);
    assert_str_equal(input, "fred fred spaz");

}

void replace_str_expand_pid_test1() {
    char input[50] = "$$";
    pid_t pid = getpid();
    char pid_as_str[10];
    sprintf(pid_as_str, "%i", pid);
    char tmp_str[50];

    replace_str(input, "$$", pid_as_str, tmp_str);

    assert_int_equal(atoi(input), pid);
}

void replace_str_expand_pid_test2() {
    char input[50] = "test$$test";
    pid_t pid= getpid();
    char pid_as_str[10];
    sprintf(pid_as_str, "%i", pid);
    char expected[100];
    sprintf(expected, "%s%i%s", "test", pid, "test");
    char tmp_str[50];

    replace_str(input, "$$", pid_as_str, tmp_str);
    /* replace_str(input, offset); */

    assert_str_equal(input, expected);
}

void replace_str_expand_pid_test3() {
    char input[100] = "$$test$$";
    pid_t pid= getpid();
    char pid_as_str[10];
    sprintf(pid_as_str, "%i", pid);
    char expected[100];
    sprintf(expected, "%i%s%i", pid, "test", pid);
    char tmp_str[100];

    replace_str(input, "$$", pid_as_str, tmp_str);

    assert_str_equal(input, expected);
}

void replace_str_expand_pid_test4() {
    char input[100] = "$$$test$$";
    pid_t pid= getpid();
    char pid_as_str[10];
    sprintf(pid_as_str, "%i", pid);
    char expected[100];
    sprintf(expected, "%i$%s%i", pid, "test", pid);
    char tmp_str[100];

    replace_str(input, "$$", pid_as_str, tmp_str);

    assert_str_equal(input, expected);
}

// }}}
// args_to_command_test {{{

void args_to_command_test_cmd_only(){
    // ARRANGE
    RawArgs args = {
        .size = 1,
        .items = {"ls"}
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_str_equal(cmd.file, "ls");
    assert_int_equal(cmd.argc, 1);
    assert_str_equal(cmd.argv[0], "ls");
    assert_int_equal(cmd.bg, 0);
}

void args_to_command_test_cmd_with_args(){
    // ARRANGE
    RawArgs args = {
        .size = 3,
        .items = {"ls", "-a", "-l"}
    };
    Command cmd = { 0 };

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_str_equal(cmd.file, "ls");
    assert_int_equal(cmd.argc, 3);
    assert_str_equal(cmd.argv[0], "ls");
    assert_str_equal(cmd.argv[1], "-a");
    assert_str_equal(cmd.argv[2], "-l");
    assert_int_equal(cmd.bg, 0);
}

void args_to_command_test_input_redirection(){
    // ARRANGE
    RawArgs args = {
        .size = 3,
        .items = {"sort", "<", "input.txt"}
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_int_equal(cmd.argc, 1);
    assert_str_equal(cmd.file, "sort");
    assert_str_equal(cmd.argv[0], "sort");
    assert_str_equal(cmd.input, "input.txt");
}

void args_to_command_test_output_redirection(){
    // ARRANGE
    RawArgs args = {
        .size = 4,
        .items = {"echo", "test", ">", "output.txt"}
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_str_equal(cmd.file, "echo");
    assert_int_equal(cmd.argc, 2);
    assert_str_equal(cmd.argv[0], "echo");
    assert_str_equal(cmd.argv[1], "test");
    assert_str_equal(cmd.output, "output.txt");
    assert_int_equal(cmd.bg, 0);
}

void args_to_command_test_input_and_output_redirection(){
    // ARRANGE
    RawArgs args = {
        .size = 6,
        .items = {"sort", "--reverse", "<", "input.txt",  ">", "output.txt"}
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_str_equal(cmd.file, "sort");
    assert_int_equal(cmd.argc, 2);
    assert_str_equal(cmd.argv[0], "sort");
    assert_str_equal(cmd.argv[1], "--reverse");
    assert_str_equal(cmd.input, "input.txt");
    assert_str_equal(cmd.output, "output.txt");
    assert_int_equal(cmd.bg, 0);
}

void args_to_command_test_full_syntax(){
    // ARRANGE
    RawArgs args = {
        .size = 7,
        .items = {"sort", "-r", "<", "input.txt",  ">", "output.txt", "&"}
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_str_equal(cmd.file, "sort");
    assert_int_equal(cmd.argc, 2);
    assert_str_equal(cmd.argv[0], "sort");
    assert_str_equal(cmd.argv[1], "-r");
    assert_str_equal(cmd.input, "input.txt");
    assert_str_equal(cmd.output, "output.txt");
    assert_int_equal(cmd.bg, 1);
}

void args_to_command_test_background_only() {
    // ARRANGE
    RawArgs args = {
        .size = 3,
        .items = { "ping", "flip", "&" }
    };
    Command cmd = {0};

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_int_equal(cmd.argc, 2);
    assert_str_equal(cmd.argv[0], "ping");
    assert_str_equal(cmd.argv[1], "flip");
    assert_int_equal(cmd.bg, 1);
}
// }}}

void run_user_input_tests() {

    read_input_test();

    input_to_args_test_blank_line();
    input_to_args_test_comment_line();
    input_to_args_test_real_input();

    replace_str_test();
    replace_str_expand_pid_test1();
    replace_str_expand_pid_test2();
    replace_str_expand_pid_test3();
    replace_str_expand_pid_test4();

    args_to_command_test_cmd_only();
    args_to_command_test_cmd_with_args();
    args_to_command_test_input_redirection();
    args_to_command_test_input_and_output_redirection();
    args_to_command_test_full_syntax();
    args_to_command_test_background_only();

}
