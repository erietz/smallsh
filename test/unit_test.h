#ifndef UNIT_TEST_H
#define UNIT_TEST_H


#define assert_int_equal(a, b) assert_int_equal_helper(a, b, __FILE__, __LINE__)
#define assert_str_equal(a, b) assert_str_equal_helper(a, b, __FILE__, __LINE__)

struct Results {
    int passed;
    int failed;
};

// pretty sure extern keyword is completely unnecessary
extern struct Results test_statistics;
void assert_int_equal_helper(int a, int b, char* filename, int linenumber);
void assert_str_equal_helper(char* a, char* b, char* filename, int linenumber);

#endif // UNIT_TEST_H
