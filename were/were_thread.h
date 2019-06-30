#ifndef WERE_THREAD_H
#define WERE_THREAD_H

#include "were_object.h"
#include "were_object_pointer.h"
#include <cstdint>
#include <sys/epoll.h> // XXX
#include <functional> // XXX


class were_thread_fd_listener
{
    friend class were_thread;
private:
    virtual void event(uint32_t events) = 0;
};

class were_thread : public were_object
{
public:
    ~were_thread();
    were_thread();

    static were_object_pointer<were_thread> current_thread() {return current_thread_;}
    int fd() const {return epoll_fd_;}

    void add_fd_listener(int fd, uint32_t events, were_thread_fd_listener *listener);
    void remove_fd_listener(int fd);

    void process(int timeout = -1);
    void run();

    std::function<void ()> idle; // FIXME

private:
    static thread_local were_object_pointer<were_thread> current_thread_;
    int epoll_fd_;
};

#endif // WERE_THREAD_H
