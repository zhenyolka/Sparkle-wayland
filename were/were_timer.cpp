#include "were_timer.h"
#include "were_exception.h"
#include <unistd.h>
#include <sys/timerfd.h>


were_timer::~were_timer()
{
    //thread()->remove_fd_listener(fd_);
    close(fd_);
}

were_timer::were_timer(int interval, bool single_shot) :
    interval_(interval), single_shot_(single_shot)
{
    MAKE_THIS_WOP

    fd_ = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this_wop);
    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->thread()->remove_fd_listener(this_wop->fd_, this_wop);
    });
}

void were_timer::start()
{
    struct itimerspec new_value;

    new_value.it_value.tv_sec = interval_ / 1000;
    new_value.it_value.tv_nsec = (interval_ % 1000) * 1000000;

    if (single_shot_)
    {
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = 0;
    }
    else
    {
        new_value.it_interval.tv_sec = interval_ / 1000;
        new_value.it_interval.tv_nsec = (interval_ % 1000) * 1000000;
    }

    if (timerfd_settime(fd_, 0, &new_value, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_timer::stop()
{
    struct itimerspec new_value;

    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(fd_, 0, &new_value, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_timer::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t expirations;

        if (read(fd_, &expirations, sizeof(uint64_t)) != sizeof(uint64_t))
            throw were_exception(WE_SIMPLE);

        were_object::emit(were_object_pointer<were_timer>(this), &were_timer::timeout);
    }
    else
        throw were_exception(WE_SIMPLE);
}
