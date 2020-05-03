#include "were_fd.h"
#include "were_thread.h"
#include "were_global.h"
#include <unistd.h>


were_fd::~were_fd()
{
    close();
}

were_fd::were_fd(int fd, uint32_t events) :
    fd_(fd), events_(events)
{
    auto this_wop = were_pointer(this);

    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    if (events_ != 0)
    {
        thread()->register_fd(this_wop);
        were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            this_wop->close();
        });
    }
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

void were_fd::event_(uint32_t events)
{
    auto this_wop = were_pointer(this);

    were::emit(this_wop, &were_fd::event, events);
}
