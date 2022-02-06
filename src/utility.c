#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
*------------------------------------------------------------------------------
* Function: replace_str()
*
* Description:
* replace_str() is used for replacing each occurance of old_str with new_str
* in input_str.
* 
* Function Arguments:
*     @param input_str[]: An array of characters needing replacements. The
*     array should be large enough to contain all of the substitutions of
*     new_str.
*     @param *old_str: A string literal of the text that needs replaced.
*     @param *new_str: A string literal of the text that will replace old_str.
*     @param tmp_str[]: An temporary array as large as input_str.
*
* Return Value:
*     input_str is modified in-place.
*------------------------------------------------------------------------------
*/
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

