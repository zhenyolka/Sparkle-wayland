#ifndef WERE_FD_H
#define WERE_FD_H

#include "were_object.h"
#include <sys/epoll.h>

class were_fd : public were_object
{
    friend class were_thread;
public:

    ~were_fd();
    were_fd(int fd, uint32_t events = 0);

    int fd() const { return fd_; }
    uint32_t events() const { return events_; }

    void enable();
    void disable();

    ssize_t read(void *buffer, size_t count);
    ssize_t write(const void *buffer, size_t count);

signals:
    were_signal<void (uint32_t events)> event;

private:
    void event_(uint32_t events);

private:
    int fd_;
    uint32_t events_;
};

#endif // WERE_FD_H
