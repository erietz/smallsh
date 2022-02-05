#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void replace_str(char input_str[], char *old_str, char *new_str, char tmp_str[]) {
    char* rep_loc;
    int offset;
    int input_len = strlen(input_str);
    int old_len = strlen(old_str);
    int new_len = strlen(new_str);

    while ((rep_loc = strstr(input_str, old_str)) != NULL ) {
        offset = rep_loc - input_str;
        strncpy(tmp_str, input_str, offset);
        tmp_str[offset] = '\0';
        strncat(tmp_str, new_str, new_len);
        strncat(tmp_str, rep_loc + old_len, input_len - offset - old_len);
        strncpy(input_str, tmp_str, input_len + new_len + 1);
    }
}

