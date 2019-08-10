#include "were_thread.h"
#include "were_exception.h"
#include <unistd.h>
#include <sys/eventfd.h>


const int MAX_EVENTS = 16;

thread_local were_object_pointer<were_thread> were_thread::current_thread_;

were_thread::~were_thread()
{
    //remove_fd_listener(event_fd_); // XXX1

    close(event_fd_);
    close(epoll_fd_);
}

were_thread::were_thread()
{
    MAKE_THIS_WOP

    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1)
        throw were_exception(WE_SIMPLE);

    event_fd_ = eventfd(0, 0);
    if (event_fd_ == -1)
        throw were_exception(WE_SIMPLE);

    add_fd_listener(event_fd_, EPOLLIN | EPOLLET, this_wop);

    if (!current_thread_)
        current_thread_ = were_object_pointer<were_thread>(this);
    else
        throw were_exception(WE_SIMPLE);
}

void were_thread::add_fd_listener(int fd, uint32_t events, were_object_pointer<were_thread_fd_listener> listener)
{
    MAKE_THIS_WOP

    listener.increment_reference_count();

    struct epoll_event event;
    event.events = events;
    event.data.ptr = listener.get();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
        throw were_exception(WE_SIMPLE);

    were::connect_x(listener, this_wop, [this_wop, fd, listener]() mutable
    {
        this_wop->remove_fd_listener(fd);
        listener.decrement_reference_count();
    });
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
    if (n == -1 && errno != EINTR)
        throw were_exception(WE_SIMPLE_ERRNO);

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

void were_thread::add_idle_handler(were_object_pointer<were_thread_idle_handler> handler)
{
    MAKE_THIS_WOP

    idle_handlers_.insert(handler);

    were::connect_x(handler, this_wop, [this_wop, handler]() mutable
    {
        this_wop->remove_idle_handler(handler);
    });
}

void were_thread::remove_idle_handler(were_object_pointer<were_thread_idle_handler> handler)
{
    idle_handlers_.erase(handler);
}

void were_thread::idle()
{
    for (auto it = idle_handlers_.begin(); it != idle_handlers_.end(); ++it)
        (*it)->idle();
}

void were_thread::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t counter = 0;
        if (read(event_fd_, &counter, sizeof(uint64_t)) != sizeof(uint64_t))
            throw were_exception(WE_SIMPLE);

        for (unsigned int i = 0; i < counter; ++i)
        {
            call_queue_mutex_.lock();
            std::function<void ()> call = call_queue_.front();
            call_queue_.pop();
            call_queue_mutex_.unlock();
            call();
        }

    }
    else
        throw were_exception(WE_SIMPLE);
}

void were_thread::post(const std::function<void ()> &call)
{
    call_queue_mutex_.lock();
    call_queue_.push(call);
    call_queue_mutex_.unlock();

    uint64_t add = 1;
    if (write(event_fd_, &add, sizeof(uint64_t)) != sizeof(uint64_t))
        throw were_exception(WE_SIMPLE);
}
