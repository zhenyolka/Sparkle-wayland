#ifndef WERE_THREAD_H
#define WERE_THREAD_H

#include "were_pointer.h"
#include "were_capability_thread.h"
#include "were_registry.h"
#include "were_handler.h"
#include "were_connect.h"
#include <cstdint>
#include <sys/epoll.h> // XXX3
#include <functional>
#include <atomic>



class were_fd;
class were_handler;

class were_thread : virtual public were_capability_rc,
                    virtual public were_capability_thread,
                    virtual public were_capability_collapse,
                    virtual public were_capability_debug
{
public:
    virtual ~were_thread();
    were_thread();

    int fd() const { return epoll_fd_; }

    void register_fd(were_pointer<were_fd> fd);
    void unregister_fd(were_pointer<were_fd> fd);

    void process_events(int timeout = -1);

    void run();
    void run_once();
    void run_for(int ms);

    bool collapsed() const override { return collapsed_; }
    void collapse() override
    {
        auto this_wop = were_pointer(this);

        were::emit(this_wop, &were_object::destroyed);
        collapsed_ = true;
    }
    void reference() override { reference_count_++; }
    void unreference() override
    {
        if (reference_count_ == 1 && collapsed_)
            delete this;
        else
            reference_count_--;
    }
    int reference_count() const override { return reference_count_.load(); }
    were_pointer<were_thread> thread() const override;

    void exit() { exit_ = true; }

    were_pointer<were_handler> handler() const { return handler_.value(); }
    void set_handler(were_pointer<were_handler> handler) { handler_ = handler; }

    std::string dump() const override;

signals:
    were_signal<void ()> idle;


private:
    std::atomic<int> reference_count_;
    bool collapsed_;
    int epoll_fd_;
    bool exit_;
    std::optional<were_pointer<were_handler>> handler_;
};

#endif // WERE_THREAD_H
