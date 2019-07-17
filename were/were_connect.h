#ifndef WERE_CONNECT_H
#define WERE_CONNECT_H

#include <cstdint>
#include <cstdio>

template <typename T>
class were_object_pointer;


namespace were
{

uint64_t next_id();

template <typename SourceType, typename SignalType, typename ContextType>
void break_(SourceType source, SignalType signal, ContextType context, uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
{
    auto signal__ = &((source.operator->())->*signal);
    signal__->remove_connection(pc_id);

    auto signal1__ = &((source.operator->())->destroyed);
    signal1__->remove_connection(sb_id);

    auto signal2__ = &((context.operator->())->destroyed);
    signal2__->remove_connection(cb_id);
}


template <typename SourceType, typename SignalType, typename ContextType, typename Functor>
void connect(   were_object_pointer<SourceType> source,
                SignalType signal,
                were_object_pointer<ContextType> context,
                Functor call
)
{
    // XXXT Thread.

    uint64_t pc_id = next_id();
    uint64_t sb_id = next_id();
    uint64_t cb_id = next_id();

    auto signal__ = &((source.operator->())->*signal);
    signal__->add_connection(call, pc_id); // XXXT Direct.

    auto signal1__ = &((source.operator->())->destroyed);
    signal1__->add_connection([source, signal, context, pc_id, sb_id, cb_id]()
    {
        break_(source, signal, context, pc_id, sb_id, cb_id);
    }, sb_id);

    auto signal2__ = &((context.operator->())->destroyed);
    signal2__->add_connection([source, signal, context, pc_id, sb_id, cb_id]()
    {
        break_(source, signal, context, pc_id, sb_id, cb_id);
    }, cb_id);
};

template <typename SourceType, typename SignalType, typename ...Args>
void emit(  were_object_pointer<SourceType> source,
            SignalType signal,
            Args... args
)
{
    auto signal__ = &((source.operator->())->*signal);
    signal__->emit(args...);
};

};

#endif // WERE_CONNECT_H
