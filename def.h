#ifndef DEF_H
#define DEF_H

// a sample combine command:
// cat < a.txt | grep str | wc -w >> ans.txt &
#define MAX_ARG_LEN 64 // max char in each arg
#define MAX_ARG_NUM 16 // max args in each command
#define PIPE_LINE (8-1) // max command in a pipe
#define MAX_LINE 1024 // max chars in input line
#define MAX_NAME 128 // max length of filename in IO redirct

typedef struct single_command {
    char *args[MAX_ARG_NUM];
    int infd, outfd;
} SINGLE_COMMAND;

typedef struct combine_command {
    char cmd_line[MAX_LINE+1];
    SINGLE_COMMAND single_command[PIPE_LINE+1];
    char *single_command_str[PIPE_LINE+1];
    int single_command_count;
    char infile[MAX_NAME];
    char outfile[MAX_NAME];
    int is_outfile_append;
    int is_bg_command;
} COMBINE_COMMAND;

#endif