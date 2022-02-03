#include "../src/user_input.h"
#include "unit_test.h"
#include <string.h>

void args_to_command_test_cmd_only(){
    // ARRANGE
    RawArgs args = {
        .size = 1,
        .items = {"ls"}
    };
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd = {
        .file = NULL,
        .input = NULL,
        .output = NULL,
        .bg = 0,
    };

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
    Command cmd;

    // ACT
    args_to_command(&args, &cmd);

    // ASSERT
    assert_int_equal(cmd.argc, 2);
    assert_str_equal(cmd.argv[0], "ping");
    assert_str_equal(cmd.argv[1], "flip");
    assert_int_equal(cmd.bg, 1);
}

void run_user_input_tests() {
    args_to_command_test_cmd_only();
    args_to_command_test_cmd_with_args();
    args_to_command_test_input_redirection();
    args_to_command_test_input_and_output_redirection();
    args_to_command_test_full_syntax();
    args_to_command_test_background_only();
}
