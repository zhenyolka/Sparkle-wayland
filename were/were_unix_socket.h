#ifndef WERE_UNIX_SOCKET_H
#define WERE_UNIX_SOCKET_H

#include "were_object_2.h"
#include "were_thread.h" // XXX3
#include <string>


class were_unix_socket : public were_object_2, public were_thread_fd_listener
{
public:
    ~were_unix_socket();
#if 0
    were_unix_socket(const std::string &path);
#endif
    were_unix_socket(int fd);

    void disconnect(bool signal = true); // XXX2

    void send(const char *data, int size);
    void receive(char *data, int size);

    int bytes_available();

signals:
    were_signal<void ()> ready_read;
    were_signal<void ()> disconnected;

private:
    void event(uint32_t events);

private:
    int fd_;
    std::string path_;
};

#endif // WERE_UNIX_SOCKET_H
