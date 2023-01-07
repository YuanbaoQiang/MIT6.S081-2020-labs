#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"


#define MSGSIZE 16

// echo hello too | xargs echo bye
int
main(int argc, char *argv[])
{
    sleep(10);
    // buf="hello too"
    char buf[MSGSIZE];
    read(0, buf, MSGSIZE);

    // argv="xargs echo bye"
    char *xargv[MAXARG];
    int xargc = 0;
    for (int i = 1; i < argc; i++) {
        xargv[xargc] = argv[i];
        xargc++;
    }

    // concat exec args
    char *p = buf;
    for (int i = 0; i < MSGSIZE; i++) {
        if (buf[i] == '\n') {
            if (fork() == 0) {
                buf[i] = 0;
                xargv[xargc] = p;
                xargc++;
                xargv[xargc] = 0;
                xargc++;
                exec(xargv[0], xargv);
                exit(0);
            } else {
                p = &buf[i+1];
                wait(0);
            }
        }
    }
    // wait(0);
    exit(0);
}