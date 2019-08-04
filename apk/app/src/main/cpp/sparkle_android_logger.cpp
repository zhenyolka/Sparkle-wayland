#include "sparkle_android_logger.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

const char *log_file_ = "/data/data/com.sion.sparkle/log.txt"; // XXX2

bool sparkle_android_logger::redirected_ = false;

sparkle_android_logger::~sparkle_android_logger()
{
}

sparkle_android_logger::sparkle_android_logger()
{
}

void sparkle_android_logger::redirect()
{
    if (redirected_)
        return;

    int fd = open(log_file_, O_WRONLY | O_CREAT | O_APPEND);
    if (fd != -1)
    {
        chmod(log_file_, 0644);

        dup2(fd, fileno(stdout));
        dup2(fd, fileno(stderr));

        close(fd);

        setvbuf(stdout, NULL, _IOLBF, 0);
        setvbuf(stderr, NULL, _IOLBF, 0);
    }

    redirected_ = true;
}
