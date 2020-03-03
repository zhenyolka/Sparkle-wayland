#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

#include <functional>
#include <list>
#include <memory>
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

template <typename Signature> class were_signal;
template <typename ...Args>
class were_signal<void (Args... args)>
{

    using function_type = std::function<void (Args... args)>;
    using connection_type = were_signal_connection<void (Args... args)>;
    using connection_list_type = std::list<connection_type>;

public:

    were_signal()
    {
        connections_ = std::shared_ptr<connection_list_type>(new connection_list_type());
    }

    void add_connection(const function_type &call, uint64_t id)
    {
        mutex_.lock();
        std::shared_ptr<const connection_list_type> connections = atomic_load(&connections_);
        std::shared_ptr<connection_list_type> new_connections(new connection_list_type(*connections));
        new_connections->push_back(were_signal_connection<void (Args... args)>(call, id));
        std::shared_ptr<const connection_list_type> new_connections_const = new_connections;
        atomic_store(&connections_, new_connections_const);
        mutex_.unlock();
    }

    void remove_connection(uint64_t id)
    {
        mutex_.lock();
        std::shared_ptr<const connection_list_type> connections = atomic_load(&connections_);
        std::shared_ptr<connection_list_type> new_connections(new connection_list_type(*connections));
        new_connections->remove_if([id](connection_type &connection)
        {
            return connection.id() == id;
        });
        std::shared_ptr<const connection_list_type> new_connections_const = new_connections;
        atomic_store(&connections_, new_connections_const);
        mutex_.unlock();
    }

    void emit(Args... args)
    {
        std::shared_ptr<const connection_list_type> connections = atomic_load(&connections_);
        for (auto &connection : *connections)
        {
            connection.call()(args...);
        }
    }

private:
    std::shared_ptr<const connection_list_type> connections_;
    std::mutex mutex_;
};

#define signals public

#endif // WERE_SIGNAL_H
