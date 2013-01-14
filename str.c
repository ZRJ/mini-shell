#include <string.h>
#include <stdio.h>
#include "str.h"

int is_trim_char(char ch);

/**
 * trim a string
 * will change the value of the input param
 * can not use it on data segment variables
 */
void trim(char *str) {
    char *p_end = NULL;
    for (p_end = str + strlen(str) - 1; is_trim_char(*p_end); p_end--);
    *(p_end+1) = '\0';
    char *p_start = NULL;
    for (p_start = str; is_trim_char(*p_start); p_start++);
    memmove(str, p_start, strlen(p_start)+1);    
}

inline int is_trim_char(char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

/**
 * explode a string by the given spliter
 * return the count of exploded elements
 */
int explode(const char *str, char spliter, char *exploded[], int max_split) {
    int exploded_count = 0, explode_write = 0;
    for (const char *tmp = str; *tmp != '\0'; tmp++) {
        if (*tmp == spliter && exploded_count < max_split - 1) {
            exploded[exploded_count][explode_write++] = '\0';
            exploded_count++;
            explode_write = 0;
        } else {
            exploded[exploded_count][explode_write++] = *tmp;
        }
    }
    return exploded_count+1;
}