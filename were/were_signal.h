#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

#include "were_exception.h"
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
    were_signal() : single_shot_(false), emit_(false) {}

    void add_connection(const std::function<void (Args...)> &call, uint64_t id);
    void remove_connection(uint64_t id);
    void emit(Args... args);

    void set_single_shot(bool single_shot)
    {
        single_shot_ = single_shot;
    }

private:
    std::list<connection_type> connections_;
    bool single_shot_;
    bool emit_; // XXX
};


template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::add_connection(const std::function<void (Args...)> &call, uint64_t id)
{
    if (emit_)
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
    if (emit_ && single_shot_)
        return;

    if (emit_)
        throw were_exception(WE_SIMPLE);

    for (auto it = connections_.begin(); it != connections_.end(); ++it)
    {
        if ((*it).id_ == id)
        {
            connections_.erase(it);
            break;
        }
    }
}

template <typename Signature> class were_signal;
template <typename ...Args>
void were_signal<void (Args...)>::emit(Args... args)
{
    //emit_ = true;

    std::list<connection_type> connections_copy = connections_; // XXX

    for (auto it = connections_copy.begin(); it != connections_copy.end(); ++it)
    {
        (*it).call_(args...);
    }

    if (single_shot_)
        connections_.clear();

    emit_ = false;
}

#define signals public

#endif // WERE_SIGNAL_H
