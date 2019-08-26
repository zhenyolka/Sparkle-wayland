#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

#include "were_exception.h"
#include "were_debug.h"
#include <functional>
#include <cstdint>
#include <list>


template <typename Signature> class were_signal_connection;
template <typename ...Args>
class were_signal_connection<void (Args...)>
{
public:
    std::function<void (Args...)> call_;
    uint64_t id_;
};


template <typename Signature> class were_signal;
template <typename ...Args>
class were_signal<void (Args...)>
{
    typedef were_signal_connection<void (Args...)> connection_type;
public:
    were_signal() {}

    void add_connection(const std::function<void (Args...)> &call, uint64_t id);
    void remove_connection(uint64_t id);
    void emit(Args... args);

private:
    std::list<connection_type> connections_;
};


template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::add_connection(const std::function<void (Args...)> &call, uint64_t id)
{
    connection_type connection;
    connection.call_ = call;
    connection.id_ = id;

    connections_.push_back(connection);
    were_debug::instance().add_connection();
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::remove_connection(uint64_t id)
{
    for (auto it = connections_.begin(); it != connections_.end(); ++it)
    {
        if ((*it).id_ == id)
        {
            connections_.erase(it);
            were_debug::instance().remove_connection();
            break;
        }
    }
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::emit(Args... args)
{
    for (auto it = connections_.begin(); it != connections_.end(); ++it)
        (*it).call_(args...);
}

#define signals public

#endif // WERE_SIGNAL_H
