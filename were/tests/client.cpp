#include <cstdio>
#include "were1_unix_socket.h"
#include <unistd.h>

#define NTESTS 10

int main(int argc, char *argv[])
{
    int fd[NTESTS];

    for (int i = 0; i < NTESTS; ++i)
    {
        fd[i] = were1_unix_socket_connect("/tmp/serva");
        if (fd[i] == -1)
        {
            fprintf(stdout, "err\n");
            return -1;
        }

        fprintf(stdout, "connected\n");

        were1_unix_socket_destroy(fd[i]);

        fprintf(stdout, "disconnected\n");
    }

    usleep(10000000);

    fprintf(stdout, "finished\n");

    return 0;
}
