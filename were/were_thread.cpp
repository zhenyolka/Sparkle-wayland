#include "were_thread.h"
#include "were_exception.h"
#include "were_fd.h"
#include "were_deadline_timer.h"
#include <unistd.h>


static const int MAX_EVENTS = 16;


were_thread::~were_thread()
{
    close(epoll_fd_);
}

were_thread::were_thread() :
    exit_(false)
{
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::register_fd(were_pointer<were_fd> fd)
{
    auto this_wop = were_pointer(this);

    fd.increment_reference_count();

    struct epoll_event event = {};
    event.events = fd->events();
    event.data.ptr = fd.access();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd->fd(), &event) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::unregister_fd(were_pointer<were_fd> fd)
{
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd->fd(), NULL) == -1)
        throw were_exception(WE_SIMPLE);

    fd.decrement_reference_count();
}

void were_thread::process_events(int timeout)
{
    struct epoll_event events[MAX_EVENTS];

    int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, timeout);
    if (n == -1 && errno != EINTR)
        throw were_exception(WE_SIMPLE_ERRNO);

    for (int i = 0; i < n; ++i)
    {
        were_fd *fd = reinterpret_cast<were_fd *>(events[i].data.ptr);
        fd->event_(events[i].events);
    }
}

void were_thread::run()
{
    auto this_wop = were_pointer(this);

    //process_queue(); // XXX1


    while (!exit_)
    {
        process_events(-1);
        were::emit(this_wop, &were_thread::idle);
    }
}

void were_thread::run_once()
{
    auto this_wop = were_pointer(this);

    process_events(0);
    were::emit(this_wop, &were_thread::idle);
}

void were_thread::run_for(int ms)
{
    auto this_wop = were_pointer(this);

    were_deadline_timer deadline(ms);

    for (;;)
    {
        int remaining = deadline.remaining_time();
        if (remaining == 0)
            break;

        process_events(remaining);
        were::emit(this_wop, &were_thread::idle);
    }
}

were_pointer<were_thread> were_thread::thread() const
{
    were_pointer<were_thread> this_wop(const_cast<were_thread *>(this));

    return this_wop;
}

std::string were_thread::dump() const
{
    char buffer[1024];
    snprintf(buffer, 1024, "%-20p%-45.44s%-5d%-10s", this, typeid(*this).name(), reference_count(), "?");

    return std::string(buffer);
}
