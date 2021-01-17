#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char *args[MAXARG], buf[100], *p;
    int new_argc = 0, i, pid;

    for (i = 1; i < argc; ++i)
    {
        args[i - 1] = (char *)malloc(strlen(argv[i]) + 1);
        memmove(args[i - 1], argv[i], strlen(argv[i]) + 1);
        ++new_argc;
    }

    p = buf;
    while (1)
    {
        /* if it reaches EOF, read() will return 0 */
        if (read(0, p, sizeof(char)) <= 0)
            break;
        if (*p == ' ')
        {
            if (new_argc >= MAXARG)
            {
                fprintf(2, "xargs: the number of arguments exceeds the limit <%d>\n", MAXARG);
                exit(1);
            }
            args[new_argc] = (char *)malloc(p - buf + 1);
            memmove(args[new_argc], buf, p - buf);
            args[new_argc][p - buf] = 0;
            ++new_argc;
            p = buf;
        }
        else if (*p == '\n')
        {
            if (new_argc >= MAXARG)
            {
                fprintf(2, "xargs: the number of arguments exceeds the limit <%d>\n", MAXARG);
                exit(1);
            }
            if (p != buf)
            {
                args[new_argc] = (char *)malloc(p - buf + 1);
                memmove(args[new_argc], buf, p - buf);
                args[new_argc][p - buf] = 0;
                ++new_argc;
                p = buf;
            }
            args[new_argc] = 0;

            if ((pid = fork()) < 0)
            {
                fprintf(2, "xargs: error when forking a new process\n");
                exit(1);
            }
            else if (pid == 0)
            {
                exec(args[0], args);
                exit(1);
            }
            wait(0);
            for (--new_argc; new_argc >= argc - 1; --new_argc)
                free(args[new_argc]);
            ++new_argc;
        }
        else
            ++p;
    }
    exit(0);
}