#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int parent_to_child[2];
    int child_to_parent[2];
    int pid = -1;
    uint8 byte;

    if (pipe(parent_to_child) < 0)
    {
        fprintf(2, "pingpong: error when creating pipes\n");
        exit(1);
    }
    if (pipe(child_to_parent) < 0)
    {
        fprintf(2, "pingpong: error when creating pipes\n");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        fprintf(2, "pingpong: error when forking child process\n");
        exit(1);
    }
    else if (pid == 0)
    {
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        read(parent_to_child[0], &byte, 1);
        printf("%d: received ping\n", getpid());

        write(child_to_parent[1], &byte, 1);
        close(parent_to_child[0]);
        close(child_to_parent[1]);
    }
    else
    {
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        byte = 111;
        write(parent_to_child[1], &byte, 1);

        read(child_to_parent[0], &byte, 1);
        printf("%d: received pong\n", getpid());

        close(parent_to_child[1]);
        close(child_to_parent[0]);
    }
    exit(0);
}