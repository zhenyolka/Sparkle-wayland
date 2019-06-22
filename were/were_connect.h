#ifndef WERE_CONNECT_H
#define WERE_CONNECT_H

//#include "were_object_pointer.h"
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
    source->thread()->post([source, signal, id]()
    {
        auto signal__ = &((source.operator->())->*signal);
        signal__->remove_connection(id);
    });
};

template <typename SourceType, typename SignalType, typename ContextType, typename Functor>
void connect(   were_object_pointer<SourceType> source,
                SignalType signal,
                were_object_pointer<ContextType> context,
                Functor call
)
{
    source->thread()->post([source, signal, context, call]()
    {
        auto signal__ = &((source.operator->())->*signal);
        uint64_t id = signal__->add_connection(call); // XXX Direct.

        context->thread()->post([source, signal, context, id]() // XXX
        {
            auto signal__ = &((context.operator->())->destroyed);
            signal__->add_connection([source, signal, id]()
            {
                disconnect(source, signal, id);
            });
        });
    });
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
