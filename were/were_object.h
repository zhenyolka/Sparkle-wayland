#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_object_pointer.h"
#include "were_signal.h"
#include "were_thread.h"
#include <atomic>



template <typename T>
were_object_pointer<T> &global();

template <typename T>
void global_set(const were_object_pointer<T> &v);

template <typename T>
void global_clear();

template <typename T>
were_object_pointer<T> &t_l_global();

template <typename T>
void t_l_global_set(const were_object_pointer<T> &v);

template <typename T>
void t_l_global_clear();


class were_object : public were_object_base
{
public:

    ~were_object();
    were_object();

    bool collapsed() const override { return collapsed_; }

    void collapse() override
    {
        auto this_wop = make_wop(this);

        were_object::emit(this_wop, &were_object::destroyed);
        collapsed_ = true;
    }

    void access() const override
    {
        if (t_l_global<were_thread>() != thread())
            throw were_exception(WE_SIMPLE);
    }

    void reference() override { reference_count_++; }
    void unreference() override
    {
        if (reference_count_ == 1 && collapsed_)
        {
            if (t_l_global<were_thread>() == thread())
                delete this;
            else
                post([this](){ delete this; });
        }
        else
            reference_count_--;
    }
    int reference_count() const override { return reference_count_.load(); }
    were_object_pointer<were_thread> thread() const override { return thread_; }
    void post(const std::function<void ()> &call) override { thread()->post(call); }



    void link(were_object_pointer<were_object> other);

    static uint64_t next_id()
    {
        return next_id_++;
    }

    template <typename SourceType, typename SignalType, typename Functor, typename ...Args>
    static void connect(    were_object_pointer<SourceType> source,
                            were_signal<void (Args...)> SignalType::*signal,
                            were_object_pointer<were_object> context,
                            Functor call)
    {
        uint64_t pc_id = next_id();
        uint64_t sb_id = next_id();
        uint64_t cb_id = next_id();

        std::function<void (Args...)> call__;

        if (context.thread() == source.thread())
            call__ = std::function<void (Args...)>(call);
        else
            call__ = make_queued_call(std::function<void (Args...)>(call), context);

        std::function<void ()> breaker = make_breaker(source, signal, context, pc_id, sb_id, cb_id);

        auto signal1__ = &((source.access_UNSAFE())->*signal);
        auto signal2__ = &((source.access_UNSAFE())->destroyed);
        auto signal3__ = &((context.access_UNSAFE())->destroyed);

        add_connection_safe(signal1__, call__, pc_id, source);
        add_connection_safe(signal2__, breaker, sb_id, source);
        add_connection_safe(signal3__, breaker, cb_id, context);
    };

    template <typename SourceType, typename SignalType>
    static void disconnect( were_object_pointer<SourceType> source,
                            SignalType signal,
                            were_object_pointer<were_object> context,
                            uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
    {
        auto signal1__ = &((source.access_UNSAFE())->*signal);
        auto signal2__ = &((source.access_UNSAFE())->destroyed);
        auto signal3__ = &((context.access_UNSAFE())->destroyed);

        remove_connection_safe(signal1__, pc_id, source);
        remove_connection_safe(signal2__, sb_id, source);
        remove_connection_safe(signal3__, cb_id, context);
    }

    template <typename SourceType, typename SignalType, typename ...Args>
    static void emit(   were_object_pointer<SourceType> source,
                        SignalType signal,
                        Args... args)
    {
        auto signal__ = &((source.access())->*signal);
        signal__->emit(args...);
    }


signals:
    were_signal<void ()> destroyed;

private:

    template <typename ...Args>
    static void add_connection_safe(    were_signal<void (Args...)> *signal,
                                        const std::function<void (Args...)> &call,
                                        uint64_t id,
                                        were_object_pointer<were_object> context)
    {
        if (t_l_global<were_thread>() == context.thread() && !signal->emitting())
            signal->add_connection(call, id);
        else
            context.post([context, signal, call, id]()
            {
                signal->add_connection(call, id);
            });
    }

    template <typename ...Args>
    static void remove_connection_safe( were_signal<void (Args...)> *signal,
                                        uint64_t id,
                                        were_object_pointer<were_object> context)
    {
        if (t_l_global<were_thread>() == context.thread() && !signal->emitting())
            signal->remove_connection(id);
        else
            context.post([context, signal, id]()
            {
                signal->remove_connection(id);
            });
    }

    template <typename SourceType, typename SignalType, typename ...Args>
    static std::function<void ()> make_breaker( were_object_pointer<SourceType> source,
                                                were_signal<void (Args...)> SignalType::*signal,
                                                were_object_pointer<were_object> context,
                                                uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
    {
        std::function<void ()> breaker = [source, signal, context, pc_id, sb_id, cb_id]()
        {
            were_object::disconnect(source, signal, context, pc_id, sb_id, cb_id);
        };

        return breaker;
    }

    template <typename ...Args>
    static std::function<void (Args...)> make_queued_call(  const std::function<void (Args...)> &call,
                                                            were_object_pointer<were_object> context)
    {
        std::function<void (Args...)> queued_call = [context, call](Args... args)
        {
            context.post([call, args...]()
            {
                call(args...);
            });
        };

        return queued_call;
    }

private:
    std::atomic<int> reference_count_;
    std::atomic<bool> collapsed_;
    were_object_pointer<were_thread> thread_;
    static std::atomic<uint64_t> next_id_;
};


template <typename T>
were_object_pointer<T> &global()
{
    return were_registry<were_object_pointer<T>>::get();
}

template <typename T>
void global_set(const were_object_pointer<T> &v)
{
    were_registry<were_object_pointer<T>>::set(v);

    were_object::connect(v, &were_object::destroyed, v, []()
    {
        global_clear<T>();
    });
}

template <typename T>
void global_clear()
{
    return were_registry<were_object_pointer<T>>::clear();
}

template <typename T>
were_object_pointer<T> &t_l_global()
{
    return were_t_l_registry<were_object_pointer<T>>::get();
}

template <typename T>
void t_l_global_set(const were_object_pointer<T> &v)
{
    were_t_l_registry<were_object_pointer<T>>::set(v);

    were_object::connect(v, &were_object::destroyed, v, []()
    {
        t_l_global_clear<T>();
    });
}

template <typename T>
void t_l_global_clear()
{
    return were_t_l_registry<were_object_pointer<T>>::clear();
}

#endif // WERE_OBJECT_H
