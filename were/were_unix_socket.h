#ifndef WERE_UNIX_SOCKET_H
#define WERE_UNIX_SOCKET_H

#include "were_object.h"
#include "were_thread.h"
#include <string>


class were_unix_socket : public were_object, public were_thread_fd_listener
{
public:
    ~were_unix_socket();
    were_unix_socket(int fd);

    void disconnect();

    bool send_all(const char *data, int size);
    bool receive_all(char *data, int size);

    int bytes_available() const;
    bool connected() const;

signals:
    were_signal<void ()> ready_read;
    were_signal<void ()> disconnected;

private:
    void event(uint32_t events);

private:
    int fd_;
};

#endif // WERE_UNIX_SOCKET_H
