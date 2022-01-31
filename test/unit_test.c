#include "unit_test.h"
#include <stdio.h>
#include <string.h>

// pretty sure extern keyword is completely unnecessary
extern struct Results test_statistics;

/*
 *------------------------------------------------------------------------------
 * Function: assert_int_equal_helper()
 *
 * Description:
 * Use the assert_int_equal Macro for vim quickfix error handling
 *------------------------------------------------------------------------------
 */
void assert_int_equal_helper(int a, int b, char* filename, int linenumber) {
    if (a == b) {
        test_statistics.passed += 1;
    } else {
        fprintf(stderr, "%s:%i: assert_int_equal(%d, %d) \n", filename, linenumber, a, b);
        test_statistics.failed += 1;
    }
}

/*
 *------------------------------------------------------------------------------
 * Function: assert_str_equal_helper()
 *
 * Description:
 * Use the assert_str_equal Macro for vim quickfix error handling
 *------------------------------------------------------------------------------
 */
void assert_str_equal_helper(char *a, char *b, char *filename, int linenumber) {

    int result = strcmp(a, b);

    if (result == 0) {
        test_statistics.passed += 1;
    } else {
        test_statistics.failed += 1;
        fprintf(stderr, "%s:%i: strcmp not equal \n", filename, linenumber);
        fprintf(stderr, "%s != %s\n", a, b);
    }

}

