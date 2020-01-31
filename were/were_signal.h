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
    were_signal() : emitting_(false) {}

    void add_connection(const std::function<void (Args...)> &call, uint64_t id);
    void remove_connection(uint64_t id);
    void emit(Args... args);
    bool emitting() const {return emitting_;}

private:
    std::list<connection_type> connections_;
    bool emitting_;
};


template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::add_connection(const std::function<void (Args...)> &call, uint64_t id)
{
    if (emitting_)
        throw were_exception(WE_SIMPLE);

    connection_type connection;
    connection.call_ = call;
    connection.id_ = id;

    connections_.push_back(connection);
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::remove_connection(uint64_t id)
{
    if (emitting_)
        throw were_exception(WE_SIMPLE);

    connections_.remove_if([id](connection_type &connection)
    {
        return connection.id_ == id;
    });
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::emit(Args... args)
{
    emitting_ = true;
    for (auto &connection : connections_)
        connection.call_(args...);
    emitting_ = false;
}

#define signals public

#endif // WERE_SIGNAL_H
