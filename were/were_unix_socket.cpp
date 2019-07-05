#include "were_unix_socket.h"
#include "were_exception.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <errno.h>


were_unix_socket::~were_unix_socket()
{
    thread()->remove_fd_listener(fd_);
    shutdown(fd_, SHUT_RDWR);
    close(fd_);
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
        throw were_exception(WE_SIMPLE); // XXX EINPROGRESS

    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this);
}
#endif

were_unix_socket::were_unix_socket(int fd)
{
    fd_ = fd;
    thread()->add_fd_listener(fd_, EPOLLIN, this); // XXX EPOLLET
}

void were_unix_socket::event(uint32_t events)
{
    MAKE_THIS_WOP

    //fprintf(stdout, "events %d\n", events);

    if (events == EPOLLIN)
        were::emit(this_wop, &were_unix_socket::ready_read);
    else if (events == (EPOLLIN | EPOLLHUP))
        were::emit(this_wop, &were_unix_socket::disconnected);
    else
        throw were_exception(WE_SIMPLE);
}

void were_unix_socket::send(const char *data, int size)
{
    int sent = 0;

    while (sent != size)
    {
        int r = ::send(fd_, data + sent, size - sent, 0);
        if (r == -1)
            throw were_exception(WE_SIMPLE); // XXX Go disconnected
        else
        {
            sent += r;
            //fprintf(stdout, "sent %d/%d\n", sent, size);
        }
    }
}

void were_unix_socket::receive(char *data, int size)
{
    int received = 0;

    while (received != size)
    {
        int r = ::recv(fd_, data + received, size - received, 0);
        if (r == -1)
            throw were_exception(WE_SIMPLE); // XXX Go disconnected
        else
        {
            received += r;
            //fprintf(stdout, "received %d/%d\n", received, size);
        }
    }
}