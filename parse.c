#include <stdio.h>
#include "parse.h"
#include "externs.h"
#include "init.h"

/**
 * shell main loop
 */
void shell_loop(void) {
    while (1) {
        printf("[minishell]$ ");
        fflush(stdout);
        init();
        if (read_command() == -1) {
            break;
        }
        parse_command();
        execute_command();
    }
}

/**
 * read command
 * return 0 on success, -1 on failed or 
 * reach the end of file
 */
int read_command(void) {
    if (fgets(cmdline, MAXLINE, stdin) == NULL) {
        return -1;
    }
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
