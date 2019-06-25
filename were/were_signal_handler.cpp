#include "were_signal_handler.h"
#include "were_exception.h"
#include <unistd.h>
#include <csignal>
#include <sys/signalfd.h>

were_signal_handler::~were_signal_handler()
{
    thread()->remove_fd_listener(fd_);
    close(fd_);
}

were_signal_handler::were_signal_handler()
{
    sigset_t mask;
    sigemptyset(&mask);
    //sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    fd_ = signalfd(-1, &mask, 0);
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this);
}

void were_signal_handler::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        struct signalfd_siginfo si;

        if (read(fd_, &si, sizeof(struct signalfd_siginfo)) != sizeof(struct signalfd_siginfo))
            throw were_exception(WE_SIMPLE);

        were::emit(were_object_pointer<were_signal_handler>(this), &were_signal_handler::signal, si.ssi_signo);
    }
    else
        throw were_exception(WE_SIMPLE);
}
