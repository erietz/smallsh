#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void replace_str(char input_str[], char *old_str, char *new_str, char tmp_str[]) {
    char* rep_loc;
    int offset, input_len, old_len, new_len;

    while ((rep_loc = strstr(input_str, old_str)) != NULL ) {
        // we must calculated these lengths each time through the loop as the
        // input_str length will change
        input_len = strlen(input_str);
        old_len = strlen(old_str);
        new_len = strlen(new_str);

        // index of first occurance of $$
        offset = rep_loc - input_str;

        // copy up to offset
        strncpy(tmp_str, input_str, offset);

        // strcat requires null terminated string
        tmp_str[offset] = '\0';

        // add on the new string
        strncat(tmp_str, new_str, new_len);

        // add on the rest of the string after the first occurance of $$
        strncat(tmp_str, rep_loc + old_len, input_len - old_len);

        // copy the tmp_str into the input_str with room for null terminator
        strncpy(input_str, tmp_str, input_len + new_len + 1);
    }
}

