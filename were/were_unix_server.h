#ifndef WERE_UNIX_SERVER_H
#define WERE_UNIX_SERVER_H

#include "were_object_2.h"
#include "were_thread.h" // XXX3
#include <string>


class were_unix_socket;


class were_unix_server : public were_object_2, public were_thread_fd_listener
{
public:
    ~were_unix_server();
    were_unix_server(const std::string &path);

    were_object_pointer<were_unix_socket> accept();

signals:
    were_signal<void ()> new_connection;

private:
    void event(uint32_t events);

private:
    int fd_;
    std::string path_;
};

#endif // WERE_UNIX_SERVER_H
