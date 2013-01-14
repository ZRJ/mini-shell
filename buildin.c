#include "buildin.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

void do_exit(void);
void do_cd(void);

typedef void (*CMD_HANDLER)(void);

typedef struct buildin_cmd {
    char *name;
    CMD_HANDLER handler;

} BUILDIN_CMD;

BUILDIN_CMD buildins[] = {
    {"exit", do_exit},
    {"cd", do_cd},
    {NULL, NULL}
};

/**
 * build-in command parse
 */
int buildin() {
    /*if (check("exit")) {
        do_exit();
    } else if (check("cd")) {
        do_cd();
    } else {
        return 0;
    }    
    return 1;*/
    int i = 0;
    while (buildins[i].name != NULL) {
        if (check(buildins[i].name)) {
            buildins[i].handler();
            return 1;
        }
        i++;
    }
    return 0;
}

void do_exit(void) {
    printf("exit\n");
    exit(EXIT_SUCCESS);
}

void do_cd(void) {
    printf("do cd ..\n");
}
