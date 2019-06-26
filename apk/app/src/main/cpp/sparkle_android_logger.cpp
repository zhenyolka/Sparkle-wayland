#include "sparkle_android_logger.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

const char *log_file_ = "/data/data/com.sion.sparkle/log.txt";

sparkle_android_logger::~sparkle_android_logger()
{
}

sparkle_android_logger::sparkle_android_logger()
{
    int fd = open(log_file_, O_WRONLY | O_CREAT);
    if (fd != -1)
    {
        chmod(log_file_, 0666);

        dup2(fd, fileno(stdout));
        dup2(fd, fileno(stderr));

        close(fd);

        setvbuf(stdout, NULL, _IOLBF, 0);
        setvbuf(stderr, NULL, _IOLBF, 0);
    }
}
