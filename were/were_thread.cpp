#include "were_thread.h"
#include "were_exception.h"
#include <unistd.h>

const int MAX_EVENTS = 16;

thread_local were_object_pointer<were_thread> were_thread::current_thread_;

were_thread::~were_thread()
{
    close(epoll_fd_);
}

were_thread::were_thread()
{
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1)
        throw were_exception(WE_SIMPLE);

    if (!current_thread_)
        current_thread_ = were_object_pointer<were_thread>(this);
    else
        throw were_exception(WE_SIMPLE);
}

void were_thread::add_fd_listener(int fd, uint32_t events, were_thread_fd_listener *listener)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = listener;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::remove_fd_listener(int fd)
{
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::process(int timeout)
{
    struct epoll_event events[MAX_EVENTS];

    int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, timeout);
    if (n == -1)
        throw were_exception(WE_SIMPLE);

    for (int i = 0; i < n; ++i)
    {
        were_thread_fd_listener *listener = reinterpret_cast<were_thread_fd_listener *>(events[i].data.ptr);
        listener->event(events[i].events);
    }

    idle();
}

void were_thread::run()
{
    for (;;)
        process(-1);
}

void were_thread::add_idle_handler(were_thread_idle_handler *handler)
{
    idle_handlers_.insert(handler);
}

void were_thread::remove_idle_handler(were_thread_idle_handler *handler)
{
    idle_handlers_.erase(handler);
}

void were_thread::idle()
{
    for (auto it = idle_handlers_.begin(); it != idle_handlers_.end(); ++it)
        (*it)->idle();
}
