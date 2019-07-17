#ifndef WERE_THREAD_H
#define WERE_THREAD_H

#include "were_object.h"
#include "were_object_pointer.h"
#include <cstdint>
#include <sys/epoll.h> // XXX3
#include <set>


class were_thread_fd_listener
{
    friend class were_thread;
private:
    virtual void event(uint32_t events) = 0;
};

class were_thread_idle_handler
{
    friend class were_thread;
private:
    virtual void idle() = 0;
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
    void add_idle_handler(were_thread_idle_handler *handler);
    void remove_idle_handler(were_thread_idle_handler *handler);

    void process(int timeout = -1);
    void run();

    void idle();

private:
    static thread_local were_object_pointer<were_thread> current_thread_;
    int epoll_fd_;
    std::set<were_thread_idle_handler *> idle_handlers_; // XXXT Thread safety
};

#endif // WERE_THREAD_H
