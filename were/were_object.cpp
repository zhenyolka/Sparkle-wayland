#include "were_object.h"

std::atomic<uint64_t> were_object::next_id_(1);

were_object::~were_object()
{
}

were_object::were_object() :
    reference_count_(0), collapsed_(false), thread_(were_thread::current_thread())
{
}

void were_object::link(were_object_pointer<were_object> other)
{
    auto this_wop = make_wop(this);

    were_object::connect(other, &were_object::destroyed, this_wop, [this_wop]() mutable
    {
        this_wop.collapse();
    });
}
