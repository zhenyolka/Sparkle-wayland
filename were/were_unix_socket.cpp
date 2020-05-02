#include "were_unix_socket.h"
#include "were_exception.h"
#include "were_fd.h"
#include "were1_unix_socket.h"


were_unix_socket::~were_unix_socket()
{
    fd_->collapse();
}

were_unix_socket::were_unix_socket(int fd) :
    fd_(new were_fd(fd, EPOLLIN | EPOLLET))
{
    auto this_wop = were_pointer(this);

    were::connect(fd_, &were_fd::event, this_wop, [this_wop](uint32_t events){ this_wop->event(events); });
}

void were_unix_socket::disconnect()
{
    auto this_wop = were_pointer(this);

    fd_->disable();
    were1_unix_socket_destroy(fd_->fd());
}

void were_unix_socket::event(uint32_t events)
{
    auto this_wop = were_pointer(this);

    if (events == EPOLLIN)
        were::emit(this_wop, &were_unix_socket::ready_read);
    else
    {
        were::emit(this_wop, &were_unix_socket::disconnected);
        disconnect();
    }

    /*
    !(events & EPOLLIN)
    events & EPOLLHUP
    events & EPOLLERR
    */
}

bool were_unix_socket::send_all(const char *data, int size)
{
    if (fd_->fd() == -1)
        return false;

    if (were1_unix_socket_send_all(fd_->fd(), data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_all(char *data, int size)
{
    if (fd_->fd() == -1)
        return false;

    if (were1_unix_socket_receive_all(fd_->fd(), data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::send_fds(const int *fds, int n)
{
    if (fd_->fd() == -1)
        return false;

    if (were1_unix_socket_send_fds(fd_->fd(), fds, n) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_fds(int *fds, int n)
{
    if (fd_->fd() == -1)
        return false;

    if (were1_unix_socket_receive_fds(fd_->fd(), fds, n) == -1)
        return false;

    return true;
}

int were_unix_socket::bytes_available() const
{
    if (fd_->fd() == -1)
        return 0;

    int bytes = were1_unix_socket_bytes_available(fd_->fd());

    return bytes;
}

bool were_unix_socket::connected() const
{
    return fd_->fd() != -1;
}
