#include "../src/user_input.h"
#include "../src/globals.h"
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

    read_input(input_buffer, MAX_CHARS, stream);

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
// expand_pid_test {{{

void expand_pid_test1() {
    char input[] = "$$";
    int offset = 0;
    pid_t pid = getpid();

    expand_pid(input, offset);

    assert_int_equal(atoi(input), pid);
}

void expand_pid_test2() {
    char input[] = "test$$test";
    int offset = 4;
    pid_t pid= getpid();
    char expected[100];
    sprintf(expected, "%s%i%s", "test", pid, "test");

    expand_pid(input, offset);

    assert_str_equal(input, expected);
}

void expand_pid_test3() {
    char input[] = "$$test$$";
    int offset = 0;
    pid_t pid= getpid();
    char expected[100];
    sprintf(expected, "%i%s%i", pid, "test", pid);

    expand_pid(input, offset);

    assert_str_equal(input, expected);
}

void expand_pid_test4() {
    char input[] = "$$$test$$";
    int offset = 0;
    pid_t pid= getpid();
    char expected[100];
    sprintf(expected, "%i$%s%i", pid, "test", pid);

    expand_pid(input, offset);

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

    expand_pid_test1();
    expand_pid_test2();
    expand_pid_test3();
    expand_pid_test4();

    args_to_command_test_cmd_only();
    args_to_command_test_cmd_with_args();
    args_to_command_test_input_redirection();
    args_to_command_test_input_and_output_redirection();
    args_to_command_test_full_syntax();
    args_to_command_test_background_only();
}
