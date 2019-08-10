#include "were_unix_socket.h"
#include "were_exception.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <errno.h>
#include <sys/ioctl.h>


were_unix_socket::~were_unix_socket()
{
    disconnect(false);
}

#if 0
were_unix_socket::were_unix_socket(const std::string &path) :
    path_(path)
{
    fd_ = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path_.c_str(), sizeof(name.sun_path) - 1);

    if (::connect(fd_, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        throw were_exception(WE_SIMPLE); // EINPROGRESS

    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this);
}
#endif

were_unix_socket::were_unix_socket(int fd)
{
    MAKE_THIS_WOP

    fd_ = fd;
    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this_wop);
}

void were_unix_socket::disconnect(bool signal)
{
    if (fd_ == -1)
        return;

    if (signal)
    {
        MAKE_THIS_WOP
        were::emit(this_wop, &were_unix_socket::disconnected);
    }

    //thread()->remove_fd_listener(fd_); //XXX1
    shutdown(fd_, SHUT_RDWR);
    close(fd_);
    fd_ = -1;
}

void were_unix_socket::event(uint32_t events)
{
    MAKE_THIS_WOP

    if (events == EPOLLIN)
        were::emit(this_wop, &were_unix_socket::ready_read);
    else
        disconnect();

    /*
    !(events & EPOLLIN)
    events & EPOLLHUP
    events & EPOLLERR
    */
}

void were_unix_socket::send(const char *data, int size)
{
    if (fd_ == -1)
        return;

    int sent = 0;

    while (sent != size)
    {
        int r = ::send(fd_, data + sent, size - sent, 0);
        if (r == -1)
        {
            disconnect();
            break;
        }
        else
        {
            sent += r;
        }
    }
}

void were_unix_socket::receive(char *data, int size)
{
    if (fd_ == -1)
        return;

    int received = 0;

    while (received != size)
    {
        int r = ::recv(fd_, data + received, size - received, 0);
        if (r == -1)
        {
            disconnect();
            break;
        }
        else
        {
            received += r;
        }
    }
}

int were_unix_socket::bytes_available() const
{
    if (fd_ == -1)
        return 0;

    int bytes = 0;
    if (ioctl(fd_, FIONREAD, &bytes) == -1)
        throw were_exception(WE_SIMPLE);

    return bytes;
}

bool were_unix_socket::connected() const
{
    return fd_ != -1;
}
