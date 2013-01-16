#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// for open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// for execvp
#include <unistd.h>

// for wait
#include <sys/types.h>
#include <sys/wait.h>

#include "shell.h"
#include "def.h"
#include "str.h"

void init_cmd(COMBINE_COMMAND *cmd);
void clean_up_cmd(COMBINE_COMMAND *cmd);
int read_cmd(COMBINE_COMMAND *cmd);
void print_cmd(COMBINE_COMMAND *cmd);
void parse_cmd(COMBINE_COMMAND *cmd);
int execute_cmd(COMBINE_COMMAND *cmd);
int fork_exec(SINGLE_COMMAND *cmd);

int shell_loop() {
    COMBINE_COMMAND cmd;
    while (1) {
        init_cmd(&cmd);
        if (read_cmd(&cmd) == -1) {
            break;
        }
        if (strlen(cmd.cmd_line) == 0) {
            continue;
        }
        parse_cmd(&cmd);
        // print_cmd(&cmd);
        execute_cmd(&cmd);
        clean_up_cmd(&cmd);
    } 
    return 0;
}

void init_cmd(COMBINE_COMMAND *cmd) {
    memset(cmd->cmd_line, 0, sizeof(cmd->cmd_line));
    memset(cmd->infile, 0, sizeof(cmd->infile));
    memset(cmd->outfile, 0, sizeof(cmd->outfile));
    cmd->single_command_count = 0;
    cmd->is_outfile_append = 0;
    cmd->is_bg_command = 0;
    for (int i=0; i<PIPE_LINE+1; i++) {        
        cmd->single_command_str[i] = 
            malloc(MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
        memset(cmd->single_command_str[i], 0, 
            MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
        cmd->single_command[i].infd = 0;
        cmd->single_command[i].outfd = 1;        
        for (int j=0; j<MAX_ARG_NUM; j++) {
            cmd->single_command[i].args[j] = malloc(MAX_ARG_LEN*sizeof(char));
            memset(cmd->single_command[i].args[j], 
                0, MAX_ARG_LEN*sizeof(char));
        }
    }
    // memory free in clean up function
    printf("[minishell]$ ");
    fflush(stdout);
}

void clean_up_cmd(COMBINE_COMMAND *cmd) {
    for (int i=0; i<PIPE_LINE+1; i++) {
        free(cmd->single_command_str[i]);
        cmd->single_command_str[i] = NULL;
        for (int j=0; j<MAX_ARG_NUM; j++) {            
            free(cmd->single_command[i].args[j]);
            cmd->single_command[i].args[j] = NULL;
        }
    }
}

int read_cmd(COMBINE_COMMAND *cmd) {
    if (fgets(cmd->cmd_line, MAX_LINE, stdin) == NULL) {
        return -1;
    }
    trim(cmd->cmd_line);
    return 0;
}

void print_cmd(COMBINE_COMMAND *cmd) {
    printf("cmd line: [%s]\n", cmd->cmd_line);
    printf("is bg cmd: %d\n", cmd->is_bg_command);
    printf("infile: [%s]\n", cmd->infile);
    printf("is outfile append: %d\n", cmd->is_outfile_append);
    printf("outfile: [%s]\n", cmd->outfile);
    printf("single cmd count: %d\n", cmd->single_command_count);
    for (int i=0; i<cmd->single_command_count; i++) {
        printf("single cmd: [%s]\n", cmd->single_command_str[i]);
        for (int j=0; cmd->single_command[i].args[j]!=NULL; j++) {
            printf("cmd args: [%s]\n", cmd->single_command[i].args[j]);
        }
    }
}

void parse_cmd(COMBINE_COMMAND *cmd) {
    // judge bg command
    if (*(cmd->cmd_line + strlen(cmd->cmd_line) - 1) == '&') {
        cmd->is_bg_command = 1;
        *(cmd->cmd_line + strlen(cmd->cmd_line) - 1) = '\0';
        trim(cmd->cmd_line);
    }

    cmd->single_command_count = explode(cmd->cmd_line, '|',
        cmd->single_command_str, PIPE_LINE+1);
    
    // judge outfile
    char *cmd_outfile_append_ptr = 
        strstr(cmd->single_command_str[cmd->single_command_count-1], ">>");
    if (cmd_outfile_append_ptr != NULL) {
        memmove(cmd_outfile_append_ptr, cmd_outfile_append_ptr+1,
            strlen(cmd_outfile_append_ptr+1)+1);
        cmd->is_outfile_append = 1;
    }
    if (strstr(cmd->single_command_str[cmd->single_command_count-1], ">")) {
        char *tmp[2];
        for (int i=0; i<2; i++) {
            tmp[i] = malloc(MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
            memset(tmp[i], 0, MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
        }
        explode(cmd->single_command_str[cmd->single_command_count-1], 
            '>', tmp, 2);
        for (int i=0; i<2; i++) {
            trim(tmp[i]);
        }
        strcpy(cmd->single_command_str[cmd->single_command_count-1], 
            tmp[0]);
        strcpy(cmd->outfile, tmp[1]);
        for (int i=0; i<2; i++) {
            free(tmp[i]);
            tmp[i] = NULL;
        }
    }
    
    // judge infile
    if (strstr(cmd->single_command_str[0], "<")) {
        char *tmp[2];
        for (int i=0; i<2; i++) {
            tmp[i] = malloc(MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
            memset(tmp[i], 0, MAX_ARG_NUM*MAX_ARG_LEN*sizeof(char));
        }
        explode(cmd->single_command_str[0], '<', tmp, 2);
        for (int i=0; i<2; i++) {
            trim(tmp[i]);
        }
        strcpy(cmd->single_command_str[0], tmp[0]);        
        strcpy(cmd->infile, tmp[1]);
        for (int i=0; i<2; i++) {
            free(tmp[i]);
            tmp[i] = NULL;
        }
    }

    // explode args
    for (int i=0; i<cmd->single_command_count; i++) {
        trim(cmd->single_command_str[i]);
        int tmp_args_count = explode(cmd->single_command_str[i], ' ',
            cmd->single_command[i].args, MAX_ARG_NUM);
        for (int j = tmp_args_count; j < MAX_ARG_NUM; j++) {           
            free(cmd->single_command[i].args[j]);
            cmd->single_command[i].args[j] = NULL;
        }
    }
}

int execute_cmd(COMBINE_COMMAND *cmd) {
    if (strlen(cmd->infile) != 0) {
        cmd->single_command[0].infd = open(cmd->infile, O_RDONLY);
    }
    if (strlen(cmd->outfile) != 0) {
        if (cmd->is_outfile_append) {
            cmd->single_command[cmd->single_command_count-1].outfd = 
                open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        } else {
            cmd->single_command[cmd->single_command_count-1].outfd = 
                open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        }
    }

    int last_pid = 0;
    for (int i=0; i<cmd->single_command_count; i++) {
        if (i < cmd->single_command_count-1) {
            int fds[2];
            pipe(fds);
            cmd->single_command[i+1].infd = fds[0];
            cmd->single_command[i].outfd = fds[1];
            // pipe close in clean up function
        }
        last_pid = fork_exec(&cmd->single_command[i]);
        // printf("last_pid is %d\n", last_pid);        
        if (cmd->single_command[i].infd != STDIN_FILENO) {
            close(cmd->single_command[i].infd);
            // printf("cmd %d infd closed\n", i);
        }
        if (cmd->single_command[i].outfd != STDOUT_FILENO) {
            close(cmd->single_command[i].outfd);
            // printf("cmd %d outfd closed\n", i);
        }
    }
    
    while (1) {
        int pid = wait(NULL);
        // printf("child %d ended\n", pid);
        if (pid == last_pid) {
            break;
        }
    }

    return 0;
}

int fork_exec(SINGLE_COMMAND *cmd) {
    pid_t pid = fork();
    if (pid > 0) {
        // parent
        return pid;
    } else if (pid == 0) {
        if (cmd->infd != STDIN_FILENO) {
            close(STDIN_FILENO);
            // printf("duplicating infd %d\n", cmd->infd);
            dup(cmd->infd);
        }
        if (cmd->outfd != STDOUT_FILENO) {
            close(STDOUT_FILENO);
            // printf("duplicating outfd %d\n", cmd->outfd);
            dup(cmd->outfd);
        }
        execvp(cmd->args[0], cmd->args);
        return 0;
    } else {
        return -1;
    }
}