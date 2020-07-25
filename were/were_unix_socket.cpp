#include "were_unix_socket.h"
#include "were_exception.h"
#include "were_fd.h"
#include "were1_unix_socket.h"
#include <sys/socket.h>


were_unix_socket::~were_unix_socket()
{
}

were_unix_socket::were_unix_socket(int fd) :
    were_fd(fd, EPOLLIN | EPOLLET)
{
}

void were_unix_socket::disconnect()
{
    auto this_wop = were_pointer(this);

    if (!is_open())
        return;

    were1_unix_socket_shutdown(fd());

    close();
}

bool were_unix_socket::send_all(const char *data, int size)
{
    if (!is_open())
        return false;

    if (were1_unix_socket_send_all(fd(), data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_all(char *data, int size)
{
    if (!is_open())
        return false;

    if (were1_unix_socket_receive_all(fd(), data, size) == -1)
        return false;

    return true;
}

bool were_unix_socket::send_fds(const int *fds, int n)
{
    if (!is_open())
        return false;

    if (were1_unix_socket_send_fds(fd(), fds, n) == -1)
        return false;

    return true;
}

bool were_unix_socket::receive_fds(int *fds, int n)
{
    if (!is_open())
        return false;

    if (were1_unix_socket_receive_fds(fd(), fds, n) == -1)
        return false;

    return true;
}

int were_unix_socket::bytes_available() const
{
    if (!is_open())
        return 0;

    int bytes = were1_unix_socket_bytes_available(fd());

    return bytes;
}

bool were_unix_socket::connected() const
{
    return is_open();
}

void were_unix_socket::event_(uint32_t events)
{
    auto this_wop = were_pointer(this);

    if (events == EPOLLIN)
        were::emit(this_wop, &were_fd::data_in);
    else
        were::emit(this_wop, &were_unix_socket::disconnected);
}
