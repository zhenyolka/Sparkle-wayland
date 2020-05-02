#ifndef WERE_UNIX_SERVER_H
#define WERE_UNIX_SERVER_H

#include "were.h"
#include <string>


class were_fd;
class were_unix_socket;

class were_unix_server : virtual public were_object
{
public:
    ~were_unix_server();
    were_unix_server(const std::string &path);

    were_pointer<were_unix_socket> accept();
    void reject();

signals:
    were_signal<void ()> new_connection;

private:
    void event(uint32_t events);

private:
    std::string path_;
    were_pointer<were_fd> fd_;
};

#endif // WERE_UNIX_SERVER_H
