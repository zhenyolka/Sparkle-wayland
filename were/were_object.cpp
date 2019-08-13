#include "were_object.h"

#include <cstdio>
#include <typeinfo>
#include "were_exception.h"
#include "were_thread.h"
#include "were_debug.h"

/* ================================================================================================================== */

uint64_t were_object::next_id_ = 0;

were_object::~were_object()
{
    were_debug::instance().remove_object(this);
}

were_object::were_object() :
    reference_count_(0), collapsed_(false)
{
    thread_ = were_thread::current_thread();
    destroyed.set_single_shot(true);

    were_debug::instance().add_object(this);
}

void were_object::collapse()
{
    MAKE_THIS_WOP

    were_object::emit(this_wop, &were_object::destroyed);
    collapsed_ = true;
}

void were_object::add_dependency(were_object_pointer<were_object> dependency)
{
    MAKE_THIS_WOP

    were_object::connect_x(dependency, this_wop, [this_wop]() mutable
    {
        this_wop.collapse();
    });
}

void were_object::break_x_(were_object_pointer<were_object> source, were_object_pointer<were_object> context, uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
{
    auto signal__ = &((source.were())->destroyed);
    signal__->remove_connection(pc_id);

    auto signal1__ = &((source.were())->destroyed);
    signal1__->remove_connection(sb_id);

    auto signal2__ = &((context.were())->destroyed);
    signal2__->remove_connection(cb_id);
}
