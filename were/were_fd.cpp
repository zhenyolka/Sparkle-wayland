#include "were_fd.h"
#include "were_thread.h"
#include "were_global.h"
#include <unistd.h>


were_fd::~were_fd()
{
    close(fd_);
}

were_fd::were_fd(int fd, uint32_t events) :
    fd_(fd), events_(events)
{
    auto this_wop = were_pointer(this);

    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    enable();
}

void were_fd::enable()
{
    auto this_wop = were_pointer(this);

    if (events() != 0)
    {
        thread()->register_fd(this_wop);
        were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            this_wop->disable(); // XXX1 check enabled
        });
    }
}

void were_fd::disable()
{
    auto this_wop = were_pointer(this);

    thread()->unregister_fd(this_wop);
}

void were_fd::event_(uint32_t events)
{
    auto this_wop = were_pointer(this);

    were::emit(this_wop, &were_fd::event, events);
}

ssize_t were_fd::read(void *buffer, size_t count)
{
    return ::read(fd_, buffer, count);
}

ssize_t were_fd::write(const void *buffer, size_t count)
{
    return ::write(fd_, buffer, count);
}
