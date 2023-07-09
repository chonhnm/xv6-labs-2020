#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn)) void loopIt(int *pfd) {
  int buf;
  int base;
  int needFolk = 0;
  int cfd[2];
  int cpid;

  close(pfd[1]);

  if (read(pfd[0], &buf, sizeof(buf)) == 0) {
    exit(0);
  }

  // print prime
  base = buf;
  printf("prime %d\n", base);

  while (read(pfd[0], &buf, sizeof(buf)) > 0) {
    if (buf % base) {
      needFolk = 1;
      break;
    }
  }

  if (!needFolk) {
    exit(0);
  }

  // pipe
  if (pipe(cfd) == -1) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  cpid = fork();
  if (cpid == -1) {
    fprintf(2, "fork error\n");
    exit(1);
  }

  if (cpid == 0) {
    loopIt(cfd);
  } else {
    close(cfd[0]);
    write(cfd[1], &buf, sizeof(buf));
    while (read(pfd[0], &buf, sizeof(buf)) > 0) {
      if (buf % base) {
        write(cfd[1], &buf, sizeof(buf));
      }
    }
    close(pfd[0]);
    close(cfd[1]);
    wait(0);
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  int fd[2];
  int cpid;

  printf("prime %d\n", 2);

  if (pipe(fd) == -1) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  cpid = fork();
  if (cpid == -1) {
    fprintf(2, "fork error\n");
    exit(1);
  }

  if (cpid == 0) {
    loopIt(fd);
  } else {
    close(fd[0]);
    for (int i = 3; i <= 35; i++) {
      if (i % 2) {
        write(fd[1], &i, sizeof(i));
      }
    }
    close(fd[1]);
    wait(0);
    exit(0);
  }
}