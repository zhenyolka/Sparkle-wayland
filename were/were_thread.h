#ifndef WERE_THREAD_H
#define WERE_THREAD_H

#include "were_object_pointer.h"
#include "were_registry.h"
#include <cstdint>
#include <sys/epoll.h> // XXX3
#include <set>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>


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

class were_thread : public were_object_base, public were_thread_fd_listener
{
public:
    ~were_thread();
    were_thread();

    int fd() const { return epoll_fd_; }

    void add_fd_listener(int fd, uint32_t events, were_object_pointer<were_thread_fd_listener> listener);
    void remove_fd_listener(int fd, were_object_pointer<were_thread_fd_listener> listener);
    void add_idle_handler(were_object_pointer<were_thread_idle_handler> handler);
    void remove_idle_handler(were_object_pointer<were_thread_idle_handler> handler);

    void process_events(int timeout = -1);
    void process_idle();
    void process_queue();

    void run();
    void run_once();
    void run_for(int ms);

    bool collapsed() const override { return collapsed_; }
    void collapse() override { collapsed_ = true; }
    void access() const override {}
    void reference() override { reference_count_++; }
    void unreference() override
    {
        if (reference_count_ == 1 && collapsed_)
            delete this;
        else
            reference_count_--;
    }
    int reference_count() const override { return reference_count_.load(); }
    were_object_pointer<were_thread> thread() const override;
    void post(const std::function<void ()> &call) override;

private:
    void add_fd_listener_(int fd, uint32_t events, were_thread_fd_listener *listener);
    void remove_fd_listener_(int fd);
    void event(uint32_t events) override;

private:
    std::atomic<int> reference_count_;
    bool collapsed_;
    int epoll_fd_;
    std::set< were_object_pointer<were_thread_idle_handler> > idle_handlers_;
    //std::mutex idle_handlers_mutex_;
    int event_fd_;
    std::queue< std::function<void ()> > call_queue_;
    std::mutex call_queue_mutex_;
};

#endif // WERE_THREAD_H
