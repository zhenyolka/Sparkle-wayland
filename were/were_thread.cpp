#include "were_thread.h"
#include "were_exception.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <ctime>


const int MAX_EVENTS = 16;

thread_local were_object_pointer<were_thread> were_thread::current_thread_;

were_thread::~were_thread()
{
    remove_fd_listener_(event_fd_);

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

    add_fd_listener_(event_fd_, EPOLLIN | EPOLLET, this);

    if (!current_thread_)
        current_thread_ = this_wop;
    else
        throw were_exception(WE_SIMPLE);
}

void were_thread::add_fd_listener(int fd, uint32_t events, were_object_pointer<were_thread_fd_listener> listener)
{
    MAKE_THIS_WOP

    listener.increment_reference_count();

    struct epoll_event event;
    event.events = events;
    event.data.ptr = listener.access();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::remove_fd_listener(int fd, were_object_pointer<were_thread_fd_listener> listener)
{
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw were_exception(WE_SIMPLE);

    listener.decrement_reference_count();
}

void were_thread::add_fd_listener_(int fd, uint32_t events, were_thread_fd_listener *listener)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = listener;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::remove_fd_listener_(int fd)
{
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_thread::add_idle_handler(were_object_pointer<were_thread_idle_handler> handler)
{
    MAKE_THIS_WOP

    handler.increment_reference_count();

    //idle_handlers_mutex_.lock();
    idle_handlers_.insert(handler);
    //idle_handlers_mutex_.unlock();
}

void were_thread::remove_idle_handler(were_object_pointer<were_thread_idle_handler> handler)
{
    //idle_handlers_mutex_.lock();
    idle_handlers_.erase(handler);
    //idle_handlers_mutex_.unlock();

    handler.decrement_reference_count();
}

void were_thread::process_events(int timeout)
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
}

void were_thread::process_idle()
{
    //idle_handlers_mutex_.lock();
    for (auto it = idle_handlers_.begin(); it != idle_handlers_.end(); ++it)
        (*it)->idle();
    //idle_handlers_mutex_.unlock();
}

void were_thread::process_queue()
{
    call_queue_mutex_.lock();
    while (call_queue_.size() > 0)
    {
        {
            std::function<void ()> call = call_queue_.front();
            call_queue_.pop();
            call_queue_mutex_.unlock();
            call();
        }
        call_queue_mutex_.lock();
    }
    call_queue_mutex_.unlock();
}

void were_thread::run()
{
    process_queue();

    for (;;)
    {
        process_events(1000);
        process_queue();
        process_idle();

        if (reference_count() == 1)
            break;
    }
}

void were_thread::run_once()
{
    process_events(0);
    process_queue();
    process_idle();
}

void were_thread::run_for(int ms)
{
    struct timespec ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);

    for (;;)
    {
        process_events(10);
        process_queue();
        process_idle();

        if (reference_count() == 1)
            break;

        clock_gettime(CLOCK_MONOTONIC, &ts2);
        uint64_t elapsed = 0;
        elapsed += 1000ULL * (ts2.tv_sec - ts1.tv_sec);
        elapsed += (ts2.tv_nsec - ts1.tv_nsec) / 1000000ULL;
        if (elapsed > ms)
            break;
    }
}

void were_thread::post(const std::function<void ()> &call)
{
    MAKE_THIS_WOP

    call_queue_mutex_.lock();
    call_queue_.push(call);
    call_queue_mutex_.unlock();

    if (were_thread::current_thread() != this_wop)
    {
        uint64_t add = 1;
        if (write(event_fd_, &add, sizeof(uint64_t)) != sizeof(uint64_t))
            throw were_exception(WE_SIMPLE);
    }
}

void were_thread::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t counter = 0;
        if (read(event_fd_, &counter, sizeof(uint64_t)) != sizeof(uint64_t))
            throw were_exception(WE_SIMPLE);
    }
    else
        throw were_exception(WE_SIMPLE);
}

were_object_pointer<were_thread> were_thread::thread()
{
    MAKE_THIS_WOP

    return this_wop;
}
