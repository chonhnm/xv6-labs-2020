#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char *fmtname(char *path)
{
    char *p;

    // Find first character after lash slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
    {
    }
    p++;
    return p;
}

void find(char *path, char *pattern)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s \n", path);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("find: path too long %s\n", path);
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
        {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch (st.type)
        {
        case T_DEVICE:
        case T_FILE:
            if (strcmp(fmtname(de.name), pattern) == 0)
            {
                printf("%s\n", buf);
            }
            break;

        case T_DIR:
            if (strcmp(".", de.name) == 0 ||
                strcmp("..", de.name) == 0)
            {
                continue;
            }
            find(buf, pattern);
            break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int i;
    char *path;
    int fd;
    struct stat st;

    if (argc < 2)
    {
        fprintf(2, "Usage: find [<DIR>] <FILE_NAME> ");
        exit(1);
    }

    path = argv[1];

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s \n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    if (st.type == T_DIR)
    {
        for (i = 2; i < argc; i++)
        {
            find(path, argv[i]);
        }
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            find(".", argv[i]);
        }
    }
    close(fd);
    exit(0);
}