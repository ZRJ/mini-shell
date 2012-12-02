#include "parse.h"

/**
 * shell main loop
 */
void shell_loop(void) {
    read_command();
    parse_command();
    execute_command();
}

/**
 * read command
 * return 0 on success, -1 on failed or 
 * reach the end of file
 */
int read_command(void) {
    return 0;
}

/**
 * parse command
 * return the number of command on success
 * return -1 on failed
 */
int parse_command(void) {
    return 0;
}

/**
 * execute command
 * return 0 on success or -1 on failed
 */
int execute_command(void) {
    return 0;
}
