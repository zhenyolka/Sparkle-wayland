#include "sparkle_android_logger.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


sparkle_android_logger::~sparkle_android_logger()
{
}

sparkle_android_logger::sparkle_android_logger() :
    redirected_(false)
{
}

void sparkle_android_logger::redirect_output(const std::string &file)
{
    if (redirected_)
        return;

    rename(file.c_str(), std::string(file + ".old").c_str());

    int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND);
    if (fd != -1)
    {
        fchmod(fd, 0644);

        dup2(fd, fileno(stdout));
        dup2(fd, fileno(stderr));

        close(fd);

        setvbuf(stdout, NULL, _IOLBF, 0);
        setvbuf(stderr, NULL, _IOLBF, 0);
    }

    redirected_ = true;
}
