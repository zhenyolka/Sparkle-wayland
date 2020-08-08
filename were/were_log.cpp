#include "were_log.h"
#include "were_exception.h"
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>


static int original_stdout_ = -1;
static int original_stderr_ = -1;
static bool captured_ = false;


were_log::~were_log()
{
}

were_log::were_log()
{
}

void were_log::enable_fd(int fd)
{
    add_logger([fd](std::vector<char> data)
    {
        write(fd, data.data(), data.size());
    });
}

void were_log::enable_file(const std::string &path)
{
    rename(path.c_str(), std::string(path + ".old").c_str());

    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_CLOEXEC, 0644); // NOLINT(hicpp-signed-bitwise)

    if (fd == -1)
        return;

    fchmod(fd, 0644);

    enable_fd(fd);
}

void were_log::message(const std::vector<char> &data)
{
    for (auto &logger : loggers_)
    {
        logger(data);
    }
}

void were_log::message(const char *format, va_list ap)
{
    std::vector<char> buffer;
    buffer.resize(1024);

    unsigned int n = vsnprintf(buffer.data(), buffer.size(), format, ap);

    if (n > buffer.size())
        n = buffer.size();

    buffer.resize(n);

    message(buffer);
}

void were_log::message(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    message(format, ap);
    va_end(ap);
}

void log(const std::vector<char> &data)
{
    were_slot<were_log *>::get()->message(data);
}

void log(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    were_slot<were_log *>::get()->message(format, ap);
    va_end(ap);
}

int stdout_capture()
{
    if (captured_)
        throw were_exception(WE_SIMPLE);

    captured_ = true;

    original_stdout_ = dup(fileno(stdout));
    original_stderr_ = dup(fileno(stderr));

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
        throw were_exception(WE_SIMPLE);

    dup2(pipe_fd[1], fileno(stdout));
    dup2(pipe_fd[1], fileno(stderr));

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    close(pipe_fd[1]);

    return pipe_fd[0];
}

void stdout_restore()
{
    if (!captured_)
        throw were_exception(WE_SIMPLE);

    captured_ = false;

    dup2(original_stdout_, fileno(stdout));
    dup2(original_stderr_, fileno(stderr));

    close(original_stdout_);
    close(original_stderr_);
}

int original_stdout()
{
    return original_stdout_;
}
