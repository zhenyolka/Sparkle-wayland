#ifndef WERE_THREAD_H
#define WERE_THREAD_H

#include "were_pointer.h"
#include "were_capability_rc_simple.h"
#include "were_capability_thread.h"
#include "were_slot.h"
#include "were_handler.h"
#include "were_connect.h"
#include <cstdint>
#include <sys/epoll.h> // XXX3


class were_fd;
class were_handler;

class were_thread : virtual public were_capability_rc_simple,
                    virtual public were_capability_integrator,
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

    were_pointer<were_thread> thread() const override;

    void exit() { exit_ = true; }

    were_pointer<were_handler> handler() const { return handler_.value(); }
    void set_handler(were_pointer<were_handler> handler) { handler_ = handler; }
    void unset_handler() { handler_.reset(); }

    std::string dump() const override;

signals:
    were_signal<void ()> idle;


private:
    int epoll_fd_;
    bool exit_;
    std::optional<were_pointer<were_handler>> handler_;
};

#endif // WERE_THREAD_H
