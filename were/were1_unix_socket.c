#include "were1_unix_socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>


int were1_unix_server_create(const char *path)
{
    unlink(path);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        goto error;

    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path, sizeof(name.sun_path) - 1);

    if (bind(fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        goto error;

    if (listen(fd, 4) == -1)
        goto error;

    return fd;

error:

    if (fd != -1)
        close(fd);

    return -1;
}

void were1_unix_server_destroy(const char *path, int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
    unlink(path);
}

int were1_unix_server_accept(int fd)
{
    return accept(fd, NULL, NULL);
}

int were1_unix_socket_connect(const char *path)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        goto error;

    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path, sizeof(name.sun_path) - 1);

    if (connect(fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        goto error;

    return fd;

error:

    if (fd != -1)
        close(fd);

    return -1;
}

void were1_unix_socket_destroy(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

int were1_unix_socket_send_all(int fd, const void *buffer, size_t length)
{
    size_t sent = 0;

    while (sent != length)
    {
        int r = send(fd, (char *)buffer + sent, length - sent, 0);
        if (r == -1)
            return -1;
        else
            sent += r;
    }

    return 0;
}

int were1_unix_socket_receive_all(int fd, void *buffer, size_t length)
{
    size_t received = 0;

    while (received != length)
    {
        int r = recv(fd, (char *)buffer + received, length - received, 0);
        if (r == -1)
            return -1;
        else
            received += r;
    }

    return 0;
}

int were1_unix_socket_bytes_available(int fd)
{
    int bytes = 0;
    if (ioctl(fd, FIONREAD, &bytes) == -1)
        return -1;

    return bytes;
}

int were1_unix_socket_send_fds(int fd, const int *fds, int n)
{
    char payload[1];

    struct iovec iov[1];
    iov[0].iov_base = payload;
    iov[0].iov_len = sizeof(payload);

    int controllen = CMSG_SPACE(sizeof(int) * n);
    char *control = malloc(controllen);

    struct msghdr msg;
    memset(&msg, 0, sizeof(struct msghdr));
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = controllen;

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * n);
    memcpy(CMSG_DATA(cmsg), fds, sizeof(int) * n);

    if (sendmsg(fd, &msg, 0) != 1)
    {
        free(control);
        return -1;
    }

    free(control);

    return 0;
}

int were1_unix_socket_receive_fds(int fd, int *fds, int n)
{
    char payload[1];

    struct iovec iov[1];
    iov[0].iov_base = payload;
    iov[0].iov_len = sizeof(payload);

    int controllen = CMSG_SPACE(sizeof(int) * n);
    char *control = malloc(controllen);

    struct msghdr msg;
    memset(&msg, 0, sizeof(struct msghdr));
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = controllen;

    if (recvmsg(fd, &msg, 0) != 1)
    {
        free(control);
        return -1;
    }

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    memcpy(fds, CMSG_DATA(cmsg), sizeof(int) * n);

    free(control);

    return 0;
}

#if 0
static int fd_set_blocking(int fd, int blocking)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    int ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        return -1;

    return 0;
}
#endif
