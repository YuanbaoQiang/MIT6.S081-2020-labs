#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2];
    char buf[10];
    int status;
    pipe(p);
    if (fork() == 0) {
        read(p[0], buf, 5);
        close(p[0]);
        printf("%d: received %s\n", getpid(), "ping");
        write(p[1], "pong", 5);
        close(p[1]);
        exit(0);
    } else {
        write(p[1], "ping", 5);
        close(p[1]);
        wait(&status);
        read(p[0], buf, 5);
        printf("%d: received %s\n", getpid(), "pong");
    }
    exit(0);
}