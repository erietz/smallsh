#include "unit_test.h"
#include "process_files_test.h"
#include "find_files_test.h"
#include "user_choices_test.h"
#include <stdio.h>

struct Results test_statistics;

void print_statistics(struct Results *statistics) {
    puts("");
    puts("==================================================================");
    puts("TEST RESULTS SUMMARY");
    puts("==================================================================");
    puts("");
    printf("PASSED ASSERTIONS: \t%i\n", statistics->passed);
    printf("FAILED ASSERTIONS: \t%i\n", statistics->failed);
    printf("TOTAL ASSERTIONS: \t%i\n",  statistics->passed + statistics->failed);
    puts("");
    puts("==================================================================");
}

int main() {
    test_statistics.passed = 0;
    test_statistics.failed = 0;

    run_process_files_tests();
    run_find_files_tests();
    run_user_choices_tests();

    print_statistics(&test_statistics);
    return 0;
}
