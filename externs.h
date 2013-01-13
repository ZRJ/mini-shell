#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "def.h"

extern char cmdline[MAXLINE+1];
extern char avline[MAXLINE+1];
extern char infile[MAXLINE+1];
extern char outfile[MAXLINE+1];
extern COMMAND cmd;

extern int cmd_count;
extern int backgnd;

#endif /* _EXTERNS_H_ */
