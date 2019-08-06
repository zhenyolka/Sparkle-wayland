#include "sparkle_android_logger.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


bool sparkle_android_logger::redirected_ = false;

sparkle_android_logger::~sparkle_android_logger()
{
}

sparkle_android_logger::sparkle_android_logger()
{
}

static void truncate1(const char *path, int size1, int size2)
{
    int fd;
    char *preserve = nullptr;

    fd = open(path, O_RDONLY);
    if (fd != -1)
    {
        int size = lseek(fd, 0, SEEK_END);
        if (size > size1)
        {
            lseek(fd, -size2, SEEK_END);
            preserve = new char[size2];
            read(fd, preserve, size2);
        }

        close(fd);
    }

    if (preserve != nullptr)
    {
        fd = open(path, O_WRONLY);
        if (fd != -1)
        {
            ftruncate(fd, 0);
            write(fd, preserve, size2);
            close(fd);
        }

        delete[] preserve;
    }
}

void sparkle_android_logger::redirect_output(const std::string &file)
{
    if (redirected_)
        return;

    truncate1(file.c_str(), 32768, 32768 / 2);

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
