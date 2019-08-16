#include "were_unix_socket.h"
#include "were_exception.h"
#include "were1_unix_socket.h"


were_unix_socket::~were_unix_socket()
{
}

were_unix_socket::were_unix_socket(int fd)
{
    MAKE_THIS_WOP

    fd_ = fd;
    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this_wop);

    were_object::connect_x(this_wop, this_wop, [this_wop]()
    {
        this_wop->disconnect();
    });
}

void were_unix_socket::disconnect()
{
    if (fd_ == -1)
        return;

    MAKE_THIS_WOP

    were_object::emit(this_wop, &were_unix_socket::disconnected);

    thread()->remove_fd_listener(fd_, this_wop);
    were1_unix_socket_destroy(fd_);
    fd_ = -1;
}

void were_unix_socket::event(uint32_t events)
{
    MAKE_THIS_WOP

    if (events == EPOLLIN)
        were_object::emit(this_wop, &were_unix_socket::ready_read);
    else
        disconnect();

    /*
    !(events & EPOLLIN)
    events & EPOLLHUP
    events & EPOLLERR
    */
}

bool were_unix_socket::send_all(const char *data, int size)
{
    if (fd_ == -1)
        return false;

    if (were1_unix_socket_send_all(fd_, data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_all(char *data, int size)
{
    if (fd_ == -1)
        return false;

    if (were1_unix_socket_receive_all(fd_, data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::send_fds(const int *fds, int n)
{
    if (fd_ == -1)
        return false;

    if (were1_unix_socket_send_fds(fd_, fds, n) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_fds(int *fds, int n)
{
    if (fd_ == -1)
        return false;

    if (were1_unix_socket_receive_fds(fd_, fds, n) == -1)
        return false;

    return true;
}

int were_unix_socket::bytes_available() const
{
    if (fd_ == -1)
        return 0;

    int bytes = were1_unix_socket_bytes_available(fd_);

    return bytes;
}

bool were_unix_socket::connected() const
{
    return fd_ != -1;
}
