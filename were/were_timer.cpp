#include "were_timer.h"
#include "were_exception.h"
#include "were_fd.h"
#include <unistd.h>
#include <sys/timerfd.h>


were_timer::~were_timer()
{
    fd_.collapse();
}

were_timer::were_timer(int interval, bool single_shot) :
    interval_(interval), single_shot_(single_shot),
    fd_(were_new<were_fd>(timerfd_create(CLOCK_MONOTONIC, 0), EPOLLIN | EPOLLET))
{
    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        were::connect(fd_, &were_fd::data_in, this_wop, [this_wop]()
        {
            uint64_t expirations;

            if (this_wop->fd_->read(&expirations, sizeof(uint64_t)) != sizeof(uint64_t))
                throw were_exception(WE_SIMPLE);

            were::emit(this_wop, &were_timer::timeout);
        });
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

    if (timerfd_settime(fd_->fd(), 0, &new_value, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_timer::stop()
{
    struct itimerspec new_value;

    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(fd_->fd(), 0, &new_value, NULL) == -1)
        throw were_exception(WE_SIMPLE);
}
