#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parse.h"
#include "externs.h"
#include "init.h"
#include "execute.h"

void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command();

/**
 * shell main loop
 */
void shell_loop(void) {
    while (1) {
        init();
        if (read_command() == -1) {
            break;
        }
        parse_command();
        //print_command();
        execute_command();
    }
}

/**
 * read command
 * return 0 on success, -1 on failed or 
 * reach the end of file
 */
int read_command(void) {
    // read command by line
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
    /*char *cp = cmdline;
    char *avp = avline;
    int i = 0;
    while (*cp != '\0') {
        while (*cp==' ' || *cp=='\t') {
            // left trim
            cp++;
        }
        if (*cp=='\0' || *cp=='\n') {
            break;
        }
        cmd.args[i] = avp;

        // parse args
        while (*cp != '\0'
            && *cp != ' '
            && *cp != '\t'
            && *cp != '\n') {
            *avp++ = *cp++;
        }
        *avp++ = '\0';
        printf("[%s]\n", cmd.args[i]);
        i++;
    }    
    return 0;*/
    /* a example command: cat < test.txt 
        | grep -n public > test2.txt & */
    if (check("\n")) {
        return 0;
    }
    // 1. parse a simple command
    get_command(0);
    // 2. judge the input redirect
    if (check("<")) {
        getname(infile);
    }
    // 3. judge the pipe
    int i;
    for (i=1; i<PIPELINE; i++) {
        if (check("|")) {
            get_command(i);
        } else {
            break;
        }
    }
    // 4. judghe the output redirect
    if (check(">")) {
        if (check(">")) {
            append = 1;
        }
        getname(outfile);
    }
    // 5. judge the background job
    if (check("&")) {
        backgnd = 1;
    }
    // 6. judge the end of a command
    if (check("\n")) {
        cmd_count = i;
        return cmd_count;
    } else {
        fprintf(stderr, "command line syntax err");
        return -1;
    }
}

/**
 * execute command
 * return 0 on success or -1 on failed
 */
int execute_command(void) {
    /*pid_t pid = fork();
    if (pid == -1) {
        ERR_EXIT("fork");
    }
    if (pid == 0) {
        // execvp(cmd.args[0], cmd.args);
    }
    wait(NULL);*/
    execute_disk_command();
    return 0;
}

void print_command() {
    int i;
    int j;
    printf("cmd_count = %d\n", cmd_count);
    printf("infile=[%s]\n", infile);
    printf("outfile=[%s]\n", outfile);
    for (i=0; i<cmd_count; ++i) {
        j = 0;
        while (cmd[i].args[j] != NULL) {
            printf("[%s] ", cmd[i].args[j]);
            j++;
        }
        printf("\n");
    }
}

/*
 * parse simple command into cmd[i],
 * put command param of cmdline into avline array,
 * and let the pointers in command.args[] 
 * point to params string
 */
void get_command(int i) {
    int j = 0;
    int inword;
    while (*lineptr != '\0') {
        // trim left space
        while (*lineptr == ' ' || *lineptr == '\t') {
            *lineptr++;
        }
        // pointer the i-th command j-th param to avptr
        cmd[i].args[j] = avptr;
        // get params
        while (*lineptr != '\0' 
            && *lineptr != ' '
            && *lineptr != '\t'
            && *lineptr != '>'
            && *lineptr != '<'
            && *lineptr != '|'
            && *lineptr != '&'
            && *lineptr != '\n') {
            // get params to avlilne
            *avptr++ = *lineptr++;
            inword = 1;
        }
        *avptr++ = '\0';
        switch (*lineptr) {
            case ' ':
            case '\t':
                inword = 0;
                j++;
                break;
            case '<':
            case '>':
            case '|':
            case '&':
            case '\n':
                if (inword == 0) {
                    cmd[i].args[j] = NULL;
                }
                return;
            default: /* for '\0' */
                return;
        }
    }
}

/**
 * compare the string in lineptr with str
 * return 1 on success or 0 on failed
 */
int check(const char *str) {
    char *p;
    // trim space
    while (*lineptr == ' '
        || *lineptr == '\t') {
        lineptr++;
    }
    p = lineptr;
    while (*str != '\0' && *str == *p) {
        str++;
        p++;
    }
    if (*str == '\0') {
        // lineptr should move over the matched string
        lineptr = p;
        return 1;
    }
    // lineptr remain unchanged
    return 0;
}

void getname(char *name) {
    // trim left space
    while (*lineptr == ' ' || *lineptr == '\t') {
        lineptr++;
    }
    while (*lineptr != '\0' 
            && *lineptr != ' '
            && *lineptr != '\t'
            && *lineptr != '>'
            && *lineptr != '<'
            && *lineptr != '|'
            && *lineptr != '&'
            && *lineptr != '\n') {
        *name++ = *lineptr++;
    }
    *name = '\0';
}
