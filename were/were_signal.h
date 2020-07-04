#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

#include "were_pointer.h"
#include "were_class_wrapper.h"
#include <functional>
#include <list>
#include <mutex>
#include <cstdint>



template <typename Signature> class were_signal_connection;
template <typename ...Args>
class were_signal_connection<void (Args... args)>
{
public:
    were_signal_connection(const std::function<void (Args... args)> &call, uint64_t id) :
        call_(call), id_(id) {}
    const std::function<void (Args... args)> &call() const { return call_; }
    uint64_t id() const { return id_; }
private:
    std::function<void (Args... args)> call_;
    uint64_t id_;
};

class were_signal_base
{
public:
    virtual void remove_connection(uint64_t id) = 0;
};

template <typename Signature> class were_signal;
template <typename ...Args>
class were_signal<void (Args... args)> : public were_signal_base
{

    using function_type = std::function<void (Args... args)>;
    using connection_type = were_signal_connection<void (Args... args)>;
    using connection_list_type = std::list<connection_type>;
    using wrapped_connection_list_type = were_class_wrapper<connection_list_type>;

public:

    were_signal() :
        connections_(new wrapped_connection_list_type())
    {
    }

    void add_connection(const function_type &call, uint64_t id)
    {
        mutex_.lock();
        were_pointer<const wrapped_connection_list_type> connections = connections_;
        were_pointer<wrapped_connection_list_type> new_connections(new wrapped_connection_list_type(*connections));
        new_connections->push_back(were_signal_connection<void (Args... args)>(call, id));
        connections_ = new_connections;
        mutex_.unlock();
    }

    void remove_connection(uint64_t id)
    {
        mutex_.lock();
        were_pointer<const wrapped_connection_list_type> connections = connections_;
        were_pointer<wrapped_connection_list_type> new_connections(new wrapped_connection_list_type(*connections));
        new_connections->remove_if([id](connection_type &connection)
        {
            return connection.id() == id;
        });
        connections_ = new_connections;
        mutex_.unlock();
    }

    void emit(Args... args)
    {
        were_pointer<const wrapped_connection_list_type> connections = connections_;
        for (auto &connection : *connections)
        {
            connection.call()(args...);
        }
    }

private:
    were_pointer<const wrapped_connection_list_type> connections_;
    std::mutex mutex_;
};

#define signals public

#endif // WERE_SIGNAL_H
