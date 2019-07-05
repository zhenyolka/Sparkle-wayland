#include "were_unix_server.h"
#include "were_exception.h"
#include "were_unix_socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>

#include <cstdio>


were_unix_server::~were_unix_server()
{
    thread()->remove_fd_listener(fd_);
    shutdown(fd_, SHUT_RDWR);
    close(fd_);
    unlink(path_.c_str());
}

were_unix_server::were_unix_server(const std::string &path) :
    path_(path)
{
    unlink(path_.c_str());

    fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path_.c_str(), sizeof(name.sun_path) - 1);

    if (bind(fd_, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        throw were_exception(WE_SIMPLE);

    if (listen(fd_, 4) == -1)
        throw were_exception(WE_SIMPLE);

    thread()->add_fd_listener(fd_, EPOLLIN | EPOLLET, this);
}

void were_unix_server::event(uint32_t events)
{
    MAKE_THIS_WOP

    if (events == EPOLLIN)
        were::emit(this_wop, &were_unix_server::new_connection);
    else
        throw were_exception(WE_SIMPLE);
}

were_object_pointer<were_unix_socket> were_unix_server::accept()
{
    int fd = ::accept(fd_, NULL, NULL);
    if (fd == -1)
        throw were_exception(WE_SIMPLE);

    were_object_pointer<were_unix_socket> socket(new were_unix_socket(fd));

    return socket;
}
