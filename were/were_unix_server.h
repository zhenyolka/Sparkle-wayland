#ifndef WERE_UNIX_SERVER_H
#define WERE_UNIX_SERVER_H

#include "were.h"
#include <string>


class were_unix_socket;


class were_unix_server : virtual public were_object, public were_thread_fd_listener
{
public:
    ~were_unix_server();
    were_unix_server(const std::string &path);

    were_object_pointer<were_unix_socket> accept();
    void reject();

signals:
    were_signal<void ()> new_connection;

private:
    void event(uint32_t events);

private:
    int fd_;
    std::string path_;
};

#endif // WERE_UNIX_SERVER_H
