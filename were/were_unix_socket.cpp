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
    shutdown(fd_, SHUT_RDWR);
    close(fd_);
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

bool were_unix_socket::send(const char *data, int size)
{
    if (fd_ == -1)
        return false;

    int sent = 0;

    while (sent != size)
    {
        int r = ::send(fd_, data + sent, size - sent, 0);
        if (r == -1)
        {
            disconnect();
            return false;
        }
        else
        {
            sent += r;
        }
    }

    return true;
}

bool were_unix_socket::receive(char *data, int size)
{
    if (fd_ == -1)
        return false;

    int received = 0;

    while (received != size)
    {
        int r = ::recv(fd_, data + received, size - received, 0);
        if (r == -1)
        {
            disconnect();
            return false;
        }
        else
        {
            received += r;
        }
    }

    return true;
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

bool were_unix_socket::send_fd(int *fd, int n)
{
    if (fd_ == -1)
        return false;

    char payload[1];

    struct iovec iov[1];
    iov[0].iov_base = payload;
    iov[0].iov_len = sizeof(payload);

    int controllen = CMSG_SPACE(sizeof(int) * n);
    char *control = new char[controllen];

    struct msghdr msg = {};
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = controllen;

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * n);
    std::memcpy(CMSG_DATA(cmsg), fd, sizeof(int) * n);

    if (sendmsg(fd_, &msg, 0) != 1)
    {
        disconnect();
        delete[] control;
        return false;
    }

    delete[] control;

    return true;
}

bool were_unix_socket::receive_fd(int *fd, int n)
{
    if (fd_ == -1)
        return false;

    char payload[1];

    struct iovec iov[1];
    iov[0].iov_base = payload;
    iov[0].iov_len = sizeof(payload);

    int controllen = CMSG_SPACE(sizeof(int) * n);
    char *control = new char[controllen];

    struct msghdr msg = {};
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = controllen;

    if (recvmsg(fd_, &msg, 0) != 1)
    {
        disconnect();
        delete[] control;
        return false;
    }

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    std::memcpy(fd, CMSG_DATA(cmsg), sizeof(int) * n);

    delete[] control;

    return true;
}
