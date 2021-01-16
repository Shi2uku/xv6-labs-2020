#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int num, from_left, current, pid;
    int fd1[2], fd2[2];
    int *pipe_left, *pipe_right, *tmp;

    pipe_left = fd1;
    pipe_right = fd2;

    /* Assume there is no exception! */
    pipe(pipe_left);
    pid = fork();

    if (pid == 0)
    {
        while (1)
        {
            close(pipe_left[1]);
            read(pipe_left[0], &from_left, sizeof(from_left));
            current = from_left;
            printf("prime %d\n", current);

            if (read(pipe_left[0], &from_left, sizeof(from_left)))
            {
                pipe(pipe_right);
                pid = fork();
                if (pid == 0)
                {
                    /* swap pipe_left and pipe_right */
                    tmp = pipe_left;
                    pipe_left = pipe_right;
                    pipe_right = tmp;
                }
                else
                {
                    close(pipe_right[0]);
                    if (from_left % current != 0)
                        write(pipe_right[1], &from_left, sizeof(from_left));

                    while (read(pipe_left[0], &from_left, sizeof(from_left)))
                    {
                        if (from_left % current != 0)
                            write(pipe_right[1], &from_left, sizeof(from_left));
                    }
                    close(pipe_left[0]);
                    close(pipe_right[1]);
                    wait(0);
                    exit(0);
                }
            }
            else
            {
                close(pipe_left[0]);
                exit(0);
            }
        }
    }
    else
    {
        close(pipe_left[0]);
        for (num = 2; num <= 35; ++num)
            write(pipe_left[1], &num, sizeof(num));
        close(pipe_left[1]);
        wait(0);
    }
    exit(0);
}