#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pfd[2];
    int cfd[2];
    int cpid;
    int pid;
    char buf;

    if (argc != 1)
    {
        fprintf(2, "usage: pingpong\n");
        exit(1);
    }

    if (pipe(pfd) == -1 || pipe(cfd) == -1)
    {
        fprintf(2, "pipe error");
        exit(1);
    }

    cpid = fork();
    if (cpid == -1)
    {
        fprintf(2, "fork error");
        exit(1);
    }

    if (cpid == 0)
    {
        // child read from pfd, write to cfd
        close(pfd[1]);
        close(cfd[0]);

        // read
        if (read(pfd[0], &buf, 1) > 0)
        {
            pid = getpid();
            fprintf(1, "%d: received ping\n", pid);
        }

        // write
        write(cfd[1], "q", 1);

        // close
        close(pfd[0]);
        close(cfd[1]);
        exit(0);
    }
    else
    {
        // parent write to pfd, read from cfd
        close(pfd[0]);
        close(cfd[1]);

        // write
        write(pfd[1], "p", 1);

        // read
        if (read(cfd[0], &buf, 1) > 0)
        {
            pid = getpid();
            fprintf(1, "%d: received pong\n", pid);
        }

        // close
        close(pfd[1]);
        close(cfd[0]);

        wait(0);
        exit(0);
    }
}
