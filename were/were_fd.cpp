#include "were_fd.h"
#include "were_thread.h"
#include "were_global.h"
#include <unistd.h>


were_fd::~were_fd()
{
}

were_fd::were_fd(int fd, uint32_t events) :
    fd_(fd), events_(events)
{
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        if (events_ != 0)
        {
            thread()->register_fd(this_wop);
            were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
            {
                this_wop->close();
            });
        }
    });
}

void were_fd::close()
{
    auto this_wop = were_pointer(this);

    if (fd_ == -1)
        return;

    thread()->unregister_fd(this_wop);

    ::close(fd_);

    fd_ = -1;
}

ssize_t were_fd::read(void *buffer, size_t count)
{
    return ::read(fd_, buffer, count);
}

ssize_t were_fd::write(const void *buffer, size_t count)
{
    return ::write(fd_, buffer, count);
}

std::vector<char> were_fd::read(size_t count)
{
    std::vector<char> buffer;

    buffer.resize(count);

    int n = read(buffer.data(), buffer.size());
    if (n > 0)
        buffer.resize(n);
    else
        buffer.resize(0);

    return buffer;
}

void were_fd::event_(uint32_t events)
{
    auto this_wop = were_pointer(this);

    if (events == EPOLLIN)
        were::emit(this_wop, &were_fd::data_in);
    else if (events == EPOLLOUT)
        were::emit(this_wop, &were_fd::data_out);
    else
        throw were_exception(WE_SIMPLE);
}
