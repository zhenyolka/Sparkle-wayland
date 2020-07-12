#include "were_signal_handler.h"
#include "were_exception.h"
#include "were_fd.h"
#include <unistd.h>
#include <csignal>
#include <sys/signalfd.h>

static were_pointer<were_fd> create_fd()
{
    sigset_t mask;
    sigemptyset(&mask);
    //sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    fprintf(stdout, "SIGINT blocked\n");

    int fd__ = signalfd(-1, &mask, 0);
    if (fd__ == -1)
        throw were_exception(WE_SIMPLE);

    return were_new<were_fd>(fd__, EPOLLIN | EPOLLET);
}

were_signal_handler::~were_signal_handler()
{
    sigset_t mask;
    sigemptyset(&mask);
    //sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    fprintf(stdout, "SIGINT unblocked\n");

    fd_->collapse();
}

were_signal_handler::were_signal_handler() :
    fd_(create_fd())
{
    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        were::connect(fd_, &were_fd::event, this_wop, [this_wop](uint32_t events)
        {
            if (events == EPOLLIN)
            {
                struct signalfd_siginfo si;

                if (this_wop->fd_->read(&si, sizeof(struct signalfd_siginfo)) != sizeof(struct signalfd_siginfo))
                    throw were_exception(WE_SIMPLE);

                were::emit(this_wop, &were_signal_handler::signal, si.ssi_signo);
            }
            else
                throw were_exception(WE_SIMPLE);
        });
    });
}
