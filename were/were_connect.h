#ifndef WERE_CONNECT_H
#define WERE_CONNECT_H

#include "were_capability_thread.h"
#include "were_capability_collapse.h"
#include "were_pointer.h"
#include "were_signal.h"
#include <cstdint>
#include <functional>
#include <vector>


class were_breaker
{
    struct were_breaker_item
    {
        were_pointer<were_capability_rc> object;
        were_signal_base *signal;
        uint64_t id;
    };
public:

    void add_item(const were_breaker_item &item)
    {
        items_.push_back(item);
    }

    void operator()() const
    {
        for (auto &item : items_)
        {
            item.signal->remove_connection(item.id);
        }
    }

private:
    std::vector<were_breaker_item> items_;
};


namespace were
{
    uint64_t next_id();

    template <typename ContextType, typename ...Args>
    static std::function<void (Args... args)> make_queued_call(  const std::function<void (Args... args)> &call,
                                                            were_pointer<ContextType> context)
    {
        std::function<void (Args... args)> queued_call = [context, call](Args... args)
        {
            context.template capability<were_capability_thread>()->thread()->handler()->post([call, args...]()
            {
                call(args...);
            });
        };

        return queued_call;
    }

    template <typename SourceType, typename SignalType, typename ContextType, typename Functor, typename ...Args>
    were_breaker connect(   were_pointer<SourceType> source,
                    were_signal<void (Args... args)> SignalType::*signal,
                    were_pointer<ContextType> context,
                    Functor call)
    {
        uint64_t pc_id = next_id();
        uint64_t sb_id = next_id();
        uint64_t cb_id = next_id();

        std::function<void (Args... args)> call__;

        if (context.template capability<were_capability_thread>()->thread() == source.template capability<were_capability_thread>()->thread())
            call__ = std::function<void (Args... args)>(call);
        else
            call__ = make_queued_call(std::function<void (Args... args)>(call), context);

        auto signal1__ = &((source.access_UNSAFE())->*signal);
        auto signal2__ = &((source.access_UNSAFE())->destroyed);
        auto signal3__ = &((context.access_UNSAFE())->destroyed);

        were_breaker breaker;

        breaker.add_item({source, signal1__, pc_id});
        breaker.add_item({source, signal2__, sb_id});
        breaker.add_item({context, signal3__, cb_id});

        signal1__->add_connection(call__, pc_id);
        signal2__->add_connection(breaker, sb_id);
        signal3__->add_connection(breaker, cb_id);

        return breaker;
    };

    template <typename SourceType, typename SignalType, typename ...Args>
    void emit(  were_pointer<SourceType> source,
                SignalType signal,
                Args... args)
    {
        auto signal__ = &((source.access())->*signal);
        signal__->emit(args...);
    }

    template <typename FirstType, typename SecondType>
    void link(were_pointer<FirstType> first, were_pointer<SecondType> second)
    {
        were::connect(second, &were_capability_collapse::destroyed, first, [first]()
        {
            first.collapse();
        });
    }


} // namespace were

#endif // WERE_CONNECT_H
