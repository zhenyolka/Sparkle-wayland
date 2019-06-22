#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

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
    were_signal() : next_id_(0), single_shot_(false) {}

    uint64_t add_connection(const std::function<void (Args...)> &call)
    {
        connection_type connection;
        connection.call_ = call;
        connection.id_ = next_id_++;

        connections_.push_back(connection);

        return connection.id_;
    }

    void remove_connection(uint64_t id)
    {
        for (auto it = connections_.begin(); it != connections_.end(); ++it)
        {
            if ((*it).id_ == id)
            {
                connections_.erase(it);
                break;
            }
        }
    }

    void emit(Args... args)
    {
        for (auto it = connections_.begin(); it != connections_.end(); ++it)
        {
            (*it).call_(args...);
        }

        if (single_shot_)
            connections_.clear();
    }

    void set_single_shot(bool single_shot)
    {
        single_shot_ = single_shot;
    }

private:
    std::list<connection_type> connections_;
    uint64_t next_id_;
    bool single_shot_;
};

#define signals public

#endif // WERE_SIGNAL_H
