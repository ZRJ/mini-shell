#ifndef _DEF_H_
#define _DEF_H_
#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } \
    while (0)

#define MAXLINE 1024 // max chars in input line
#define MAXARG 20 // max args in each command
#define PIPELINE 5 // max command in a pipe
#define MAXNAME 100 // max length of filename in IO redirct

typedef struct command {
    // the command args after parse
    char *args[MAXARG+1];
    int infd;
    int outfd;
} COMMAND;

#endif /* _DEF_H_ */
