#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

#include "were_exception.h"
#include "were_debug.h"
#include "were_registry.h"
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

    were_debug *debug = were_registry<were_debug>::get();
    if (debug)
        debug->add_connection();
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::remove_connection(uint64_t id)
{
    if (emitting_)
        throw were_exception(WE_SIMPLE);

    for (auto it = connections_.begin(); it != connections_.end(); ++it)
    {
        if ((*it).id_ == id)
        {
            connections_.erase(it);

            were_debug *debug = were_registry<were_debug>::get();
            if (debug)
                debug->remove_connection();

            break;
        }
    }
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::emit(Args... args)
{
    emitting_ = true;
    for (auto it = connections_.begin(); it != connections_.end(); ++it)
        (*it).call_(args...);
    emitting_ = false;
}

#define signals public

#endif // WERE_SIGNAL_H
