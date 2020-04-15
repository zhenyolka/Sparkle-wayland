#include "were_unix_server.h"
#include "were_exception.h"
#include "were1_unix_socket.h"
#include "were_unix_socket.h"



were_unix_server::~were_unix_server()
{
    were1_unix_server_destroy(path_.c_str(), fd_);
}

were_unix_server::were_unix_server(const std::string &path) :
    path_(path)
{
    auto this_wop = were_pointer(this);

    fd_ = were1_unix_server_create(path_.c_str());
    if (fd_ == -1)
        throw were_exception(WE_SIMPLE);

    thread()->add_fd_listener(fd_, EPOLLIN /* | EPOLLET */, this_wop);
    were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->thread()->remove_fd_listener(this_wop->fd_, this_wop);
    });
}

void were_unix_server::event(uint32_t events)
{
    auto this_wop = were_pointer(this);

    if (events == EPOLLIN)
        were::emit(this_wop, &were_unix_server::new_connection);
    else
        throw were_exception(WE_SIMPLE);
}

were_pointer<were_unix_socket> were_unix_server::accept()
{
    int fd = were1_unix_server_accept(fd_);
    if (fd == -1)
        throw were_exception(WE_SIMPLE);

    were_pointer<were_unix_socket> socket(new were_unix_socket(fd));

    return socket;
}

void were_unix_server::reject()
{
    were1_unix_server_reject(fd_);
}
