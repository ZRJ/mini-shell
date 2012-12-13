#include "init.h"
#include "parse.h"
#include "def.h"

char cmdline[MAXLINE+1];
COMMAND cmd;

int main() {
    setup();
    shell_loop();
    return 0;
}