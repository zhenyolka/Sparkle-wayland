#include "were_unix_server.h"
#include "were_exception.h"
#include "were_fd.h"
#include "were1_unix_socket.h"
#include "were_unix_socket.h"


were_unix_server::~were_unix_server()
{
    were1_unix_server_shutdown(path_.c_str(), fd_->fd());

    fd_->collapse();
}

were_unix_server::were_unix_server(const std::string &path) :
    path_(path),
    fd_(new were_fd(were1_unix_server_create(path_.c_str()), EPOLLIN))
{
    auto this_wop = were_pointer(this);

    were::connect(fd_, &were_fd::event, this_wop, [this_wop](uint32_t events)
    {
        if (events == EPOLLIN)
            were::emit(this_wop, &were_unix_server::new_connection);
        else
            throw were_exception(WE_SIMPLE);
    });
}

were_pointer<were_unix_socket> were_unix_server::accept()
{
    int fd = were1_unix_server_accept(fd_->fd());
    if (fd == -1)
        throw were_exception(WE_SIMPLE);

    were_pointer<were_unix_socket> socket(new were_unix_socket(fd));

    return socket;
}

void were_unix_server::reject()
{
    were1_unix_server_reject(fd_->fd());
}
