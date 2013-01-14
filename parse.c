#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parse.h"
#include "externs.h"
#include "init.h"

void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command();
void forkexec(int i);

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
        print_command();
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

    // if has infile outfile
    if (infile[0] != '\0') {
        cmd[0].infd = open(infile, O_RDONLY);
    }
    if (outfile[0] != '\0') {
        if (append) {
            cmd[cmd_count-1].outfd = open(outfile, O_WRONLY
                | O_CREAT | O_APPEND, 0666);
        } else {
            cmd[cmd_count-1].outfd = open(outfile, O_WRONLY
                | O_CREAT | O_TRUNC, 0666);
        }
    }
    // since background job will not be waited
    // so need to ingnore the signal
    if (backgnd == 1) {
        signal(SIGCHLD, SIG_IGN);
    }

    int i;
    int fd;
    int fds[2];
    for (i=0; i<cmd_count; ++i) {
        // if not the last command, create a pipe
        if (i < cmd_count-1) {
            pipe(fds);
            cmd[i].outfd = fds[1];
            cmd[i+1].infd = fds[0];
        }
        forkexec(i);
        if ((fd=cmd[i].infd) != 0) {
            close(fd);
        }
        if ((fd=cmd[i].outfd) != 1) {
            close(fd);
        }
    }

    if (backgnd == 0) {
        while (wait(NULL) != lastpid) {

        }
    }
    
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

void forkexec(int i) {
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        ERR_EXIT("fork");
    }
    if (pid > 0) {
        lastpid = pid;
        // parent
    } else if (pid == 0) {
        // redirect the first command's infd to /dev/null
        // when background = 1
        if (cmd[i].infd == 0 && backgnd == 1) {
            cmd[i].infd = open("/dev/null", O_RDONLY);
        }
        // make the first command as the process group  
        if (i == 0) {
            setpgid(0, 0);
        }    
        // forked
        if (cmd[i].infd != 0) {
            close(0);
            dup(cmd[i].infd);
        }  
        if (cmd[i].outfd != 1) {
            close(1);
            dup(cmd[i].outfd);
        }
        execvp(cmd[i].args[0], cmd[i].args);
        int i;
        for (i = 3; i < 1024; i++) {
            // OPEN_MAX removed
            close(i);
        }
        // frontground job can catch signal
        if (backgnd == 0) {
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
        }
        exit(EXIT_FAILURE);
    }
}
