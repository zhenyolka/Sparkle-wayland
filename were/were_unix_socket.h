#ifndef WERE_UNIX_SOCKET_H
#define WERE_UNIX_SOCKET_H

#include "were.h"
#include <string>

class were_fd;

class were_unix_socket : virtual public were_object
{
public:
    ~were_unix_socket();
    were_unix_socket(int fd);

    void disconnect();

    bool send_all(const char *data, int size);
    bool receive_all(char *data, int size);
    bool send_fds(const int *fds, int n);
    bool receive_fds(int *fds, int n);

    int bytes_available() const;
    bool connected() const;

signals:
    were_signal<void ()> ready_read;
    were_signal<void ()> disconnected;

private:
    were_pointer<were_fd> fd_;
};

#endif // WERE_UNIX_SOCKET_H
