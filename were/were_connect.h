#ifndef WERE_CONNECT_H
#define WERE_CONNECT_H

#include <cstdint>

template <typename T>
class were_object_pointer;

namespace were
{

template <typename SourceType, typename SignalType>
void disconnect(    were_object_pointer<SourceType> source,
                    SignalType signal,
                    uint64_t id
)
{
    // XXX Thread.

    auto signal__ = &((source.operator->())->*signal);
    signal__->remove_connection(id);
};

template <typename SourceType, typename SignalType, typename ContextType, typename Functor>
void connect(   were_object_pointer<SourceType> source,
                SignalType signal,
                were_object_pointer<ContextType> context,
                Functor call
)
{
    // XXX Thread.

    auto signal__ = &((source.operator->())->*signal);
    uint64_t id = signal__->add_connection(call); // XXX Direct.

#if 0
    auto signal2__ = &((context.operator->())->destroyed);
    signal2__->add_connection([source, signal, id]()
    {
            disconnect(source, signal, id);
    });
#endif
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
