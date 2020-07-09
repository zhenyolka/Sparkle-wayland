#include "were_log.h"
#include "were_fd.h"
#include <unistd.h>
#include <cstdio>
#include <cstdarg>
#include <fcntl.h>
#include <sys/stat.h>



were_log::~were_log()
{
    close(original_stdout_);
    close(original_stderr_);
}

were_log::were_log()
{
    original_stdout_ = dup(fileno(stdout));
    original_stderr_ = dup(fileno(stderr));
}

void were_log::capture_stdout()
{
    auto this_wop = were_pointer(this);

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
        return;

    dup2(pipe_fd[1], fileno(stdout));
    dup2(pipe_fd[1], fileno(stderr));

    close(pipe_fd[1]);

    int fd__ = pipe_fd[0];

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    were_pointer<were_fd> fd = were_new<were_fd>(fd__, EPOLLIN);
    were::connect(fd, &were_fd::event, this_wop, [this_wop, fd](uint32_t events){ this_wop->event(fd, events); });
    were::link(fd, this_wop);
}

void were_log::enable_stdout()
{
    auto this_wop = were_pointer(this);

    were::connect(this_wop, &were_log::text, this_wop, [this_wop](std::vector<char> text)
    {
        write(this_wop->original_stdout_, text.data(), text.size());
    });
}

void were_log::enable_file(const std::string &path)
{
    auto this_wop = were_pointer(this);

    rename(path.c_str(), std::string(path + ".old").c_str());

    int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0644);

    if (fd == -1)
        return;

    fchmod(fd, 0644);

    were::connect(this_wop, &were_log::text, this_wop, [this_wop, fd](std::vector<char> text)
    {
        write(fd, text.data(), text.size());
    });
}

void were_log::event(were_pointer<were_fd> fd, uint32_t events)
{
    auto this_wop = were_pointer(this);

    if (events == EPOLLIN)
    {
        std::vector<char> buffer;
        buffer.resize(512);
        int n = fd->read(buffer.data(), buffer.size());
        if (n > 0)
        {
            buffer.resize(n);
            were::emit(this_wop, &were_log::text, buffer);
        }
    }
    else
        throw were_exception(WE_SIMPLE);
}

void were_log::message(const char *format, va_list ap)
{
    auto this_wop = were_pointer(this);

    std::vector<char> buffer;
    buffer.resize(1024);

    unsigned int n = vsnprintf(buffer.data(), buffer.size(), format, ap);

    if (n > buffer.size())
        n = buffer.size();

    buffer.resize(n);

    were::emit(this_wop, &were_log::text, buffer);
}

void were_log::message(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    message(format, ap);
    va_end(ap);
}

void log(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    global<were_log>()->message(format, ap);
    va_end(ap);
}
