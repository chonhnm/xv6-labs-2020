#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct cmdarg {
  char *argv[MAXARG];
  char *eargv[MAXARG];
};

__attribute__((noreturn)) void runcmd(struct cmdarg *arg) {
  if (arg->argv[0] == 0) {
    fprintf(2, "no exec file\n");
    exit(1);
  }
  exec(arg->argv[0], arg->argv);
  fprintf(2, "exec %s failed\n", arg->argv[0]);
  exit(1);
}

struct cmdarg *cmdarg(void) {
  struct cmdarg *ret;
  ret = malloc(sizeof(*ret));
  memset(ret, 0, sizeof(*ret));
  return ret;
}

int readline(char *buf, int nbuf) {
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if (buf[0] == 0) { // EOF
    return -1;
  }
  return 0;
}

char whitespace[] = " \t\r\n\v";

int gettoken(char **ps, char *es, char **q, char **eq) {
  char *s;
  int ret;

  s = *ps;
  while (s < es && strchr(whitespace, *s)) {
    s++;
  }
  if (q) {
    *q = s;
  }
  ret = *s;
  while (s < es && !strchr(whitespace, *s)) {
    s++;
  }
  if (eq) {
    *eq = s;
  }
  while (s < es && strchr(whitespace, *s)) {
    s++;
  }
  *ps = s;
  if (*q == *eq) {
    return 0;
  }
  return ret;
}

void parsecmd(struct cmdarg *cmdarg, char *buf, int argc) {
  char **ps, *es;
  char *q, *eq;
  int i;

  ps = &buf;
  es = buf + strlen(buf);
  while (gettoken(ps, es, &q, &eq) > 0) {
    cmdarg->argv[argc] = q;
    cmdarg->eargv[argc] = eq;
    argc++;
    if (argc >= MAXARG) {
      fprintf(2, "too many args");
      exit(1);
    }
  }
  cmdarg->argv[argc] = 0;
  cmdarg->eargv[argc] = 0;

  // null terminate
  for (i = 0; i < argc; i++) {
    if (cmdarg->argv[i] == 0) {
      break;
    }
    if (cmdarg->eargv[i] != 0) {
      *cmdarg->eargv[i] = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  char buf[100];
  struct cmdarg *ret;
  int i;

  if (argc < 2) {
    fprintf(2, "Usage: xargs <CMD> [PARAMS]");
    exit(1);
  }

  ret = cmdarg();
  for (i = 0; i < argc - 1; i++) {
    ret->argv[i] = argv[i + 1];
  }
  while (readline(buf, sizeof(buf)) >= 0) {
    if (fork() == 0) {
      parsecmd(ret, buf, i);
      runcmd(ret);
    }
    wait(0);
  }
  exit(0);
}