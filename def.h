#ifndef _DEF_H_
#define _DEF_H_

#define MAXLINE 1024 // max chars in input line
#define MAXARG 20 // max args in each command
#define PIPELINE 5 // max command in a pipe
#define MAXNAME 100 // max length of filename in IO redirct

typedef struct command {
    // the command args after parse
    char *args[MAXARG+1];
} COMMAND;

#endif /* _DEF_H_ */
