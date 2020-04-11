#ifndef WERE_CONNECT_H
#define WERE_CONNECT_H

#include "were_capability_thread.h"
#include "were_object_pointer.h"
#include "were_object.h"
#include "were_signal.h"
#include <cstdint>
#include <functional>


namespace were
{
    uint64_t next_id();

    template <typename SourceType, typename SignalType>
    void disconnect(were_object_pointer<SourceType> source,
                    SignalType signal,
                    were_object_pointer<were_object> context,
                    uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
    {
        auto signal1__ = &((source.access_UNSAFE())->*signal);
        auto signal2__ = &((source.access_UNSAFE())->destroyed);
        auto signal3__ = &((context.access_UNSAFE())->destroyed);

        signal1__->remove_connection(pc_id);
        signal2__->remove_connection(sb_id);
        signal3__->remove_connection(cb_id);
    }

    template <typename SourceType, typename SignalType, typename ...Args>
    static std::function<void ()> make_breaker( were_object_pointer<SourceType> source,
                                                were_signal<void (Args...)> SignalType::*signal,
                                                were_object_pointer<were_object> context,
                                                uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
    {
        std::function<void ()> breaker = [source, signal, context, pc_id, sb_id, cb_id]()
        {
            were::disconnect(source, signal, context, pc_id, sb_id, cb_id);
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

    template <typename SourceType, typename SignalType, typename Functor, typename ...Args>
    void connect(   were_object_pointer<SourceType> source,
                    were_signal<void (Args...)> SignalType::*signal,
                    were_object_pointer<were_object> context,
                    Functor call)
    {
        uint64_t pc_id = next_id();
        uint64_t sb_id = next_id();
        uint64_t cb_id = next_id();

        std::function<void (Args...)> call__;

        were_object_pointer<were_object> source1 = source;

        if (context.capability<were_capability_thread>()->thread() == source1.capability<were_capability_thread>()->thread())
            call__ = std::function<void (Args...)>(call);
        else
            call__ = make_queued_call(std::function<void (Args...)>(call), context);

        std::function<void ()> breaker = make_breaker(source, signal, context, pc_id, sb_id, cb_id);

        auto signal1__ = &((source.access_UNSAFE())->*signal);
        auto signal2__ = &((source.access_UNSAFE())->destroyed);
        auto signal3__ = &((context.access_UNSAFE())->destroyed);

        signal1__->add_connection(call__, pc_id);
        signal2__->add_connection(breaker, sb_id);
        signal3__->add_connection(breaker, cb_id);
    };

    template <typename SourceType, typename SignalType, typename ...Args>
    static void emit(   were_object_pointer<SourceType> source,
                        SignalType signal,
                        Args... args)
    {
        auto signal__ = &((source.access())->*signal);
        signal__->emit(args...);
    }

}

#endif // WERE_CONNECT_H
