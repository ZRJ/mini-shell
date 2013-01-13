#include "init.h"
#include "parse.h"
#include "def.h"

char cmdline[MAXLINE+1];
char avline[MAXLINE+1];
char *lineptr;
char *avptr;

char infile[MAXLINE+1];
char outfile[MAXLINE+1];
COMMAND cmd[PIPELINE];

int cmd_count;
int backgnd;

int main() {
    setup();
    shell_loop();
    return 0;
}