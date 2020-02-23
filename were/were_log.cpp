#include "were_log.h"
#include <unistd.h>
#include <cstdio>
#include <cstdarg>
#include <fcntl.h>
#include <sys/stat.h>



were_log::~were_log()
{
    if (fd_ != -1)
        close(fd_);
    if (stdout1_ != -1)
    {
        dup2(stdout1_, fileno(stdout));
        close(stdout1_);
    }
    if (stderr1_ != -1)
    {
        dup2(stderr1_, fileno(stderr));
        close(stderr1_);
    }
}

were_log::were_log() :
    fd_(-1), stdout1_(-1), stderr1_(-1)
{
}

void were_log::capture_stdout()
{
    auto this_wop = make_wop(this);

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
        return;

    stdout1_ = dup(fileno(stdout));
    stderr1_ = dup(fileno(stderr));

    dup2(pipe_fd[1], fileno(stdout));
    dup2(pipe_fd[1], fileno(stderr));

    close(pipe_fd[1]);

    fd_ = pipe_fd[0];

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);


    thread()->add_fd_listener(fd_, EPOLLIN /* | EPOLLET */, this_wop);
    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->thread()->remove_fd_listener(this_wop->fd_, this_wop);
    });
}

void were_log::enable_stdout()
{
    auto this_wop = make_wop(this);

    were_object::connect(this_wop, &were_log::text, this_wop, [this_wop](std::vector<char> text)
    {
        write(this_wop->stdout1_, text.data(), text.size());
    });
}

void were_log::enable_file(const std::string &path)
{
    auto this_wop = make_wop(this);

    rename(path.c_str(), std::string(path + ".old").c_str());

    int fd = open(path.c_str(), O_WRONLY | O_CREAT);

    if (fd == -1)
        return;

    fchmod(fd, 0644);

    were_object::connect(this_wop, &were_log::text, this_wop, [this_wop, fd](std::vector<char> text)
    {
        write(fd, text.data(), text.size());
    });
}

void were_log::event(uint32_t events)
{
    auto this_wop = make_wop(this);

    if (events == EPOLLIN)
    {
        std::vector<char> buffer;
        buffer.resize(512);
        int n = read(fd_, buffer.data(), buffer.size());
        if (n > 0)
        {
            buffer.resize(n);
            were_object::emit(this_wop, &were_log::text, buffer);
        }
    }
    else
        throw were_exception(WE_SIMPLE);
}

void were_log::message(const char *format, va_list ap)
{
    auto this_wop = make_wop(this);

    std::vector<char> buffer;
    buffer.resize(1024);

    unsigned int n = vsnprintf(buffer.data(), buffer.size(), format, ap);

    if (n > buffer.size())
        n = buffer.size();

    buffer.resize(n);

    were_object::emit(this_wop, &were_log::text, buffer);
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
