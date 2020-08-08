#ifndef WERE_UNIX_SOCKET_H
#define WERE_UNIX_SOCKET_H

#include "were.h"
#include "were_fd.h"


class were_unix_socket : public were_fd
{
public:
    ~were_unix_socket() override;
    explicit were_unix_socket(int fd);

    void disconnect();

    bool send_all(const char *data, int size); // XXX2 move to fd
    bool receive_all(char *data, int size);
    bool send_fds(const int *fds, int n);
    bool receive_fds(int *fds, int n);

    int bytes_available() const;
    bool connected() const;

signals:
    were_signal<void ()> disconnected;

private:
    void event_(uint32_t events) override;
};

#endif // WERE_UNIX_SOCKET_H
