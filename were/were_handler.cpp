#include "were_handler.h"
#include "were_fd.h"
#include "were_connect.h"
#include "were_thread.h"
#include <sys/eventfd.h>
#include <unistd.h>


were_handler::~were_handler()
{
    fd_.collapse();
}

were_handler::were_handler() :
    fd_(were_new<were_fd>(eventfd(0, 0), EPOLLIN | EPOLLET))
{
    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        were::connect(fd_, &were_fd::data_in, this_wop, [this_wop]()
        {
            uint64_t counter = 0;
            if (this_wop->fd_->read(&counter, sizeof(uint64_t)) != sizeof(uint64_t))
                throw were_exception(WE_SIMPLE);

            this_wop->process_queue();
        });
    });
}

void were_handler::process_queue()
{
    call_queue_mutex_.lock();
    while (call_queue_.size() > 0)
    {
        {
            std::function<void ()> call = std::move(call_queue_.front());
            call_queue_.pop();
            call_queue_mutex_.unlock();
            call();
        }
        call_queue_mutex_.lock();
    }
    call_queue_mutex_.unlock();
}

void were_handler::post(const std::function<void ()> &call)
{
    call_queue_mutex_.lock();
    call_queue_.push(call);
    call_queue_mutex_.unlock();

    uint64_t add = 1;
    if (fd_->write(&add, sizeof(uint64_t)) != sizeof(uint64_t))
        throw were_exception(WE_SIMPLE);
}
