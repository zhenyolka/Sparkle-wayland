#include "were_object.h"

#include <cstdio>
#include <typeinfo>
#include "were_exception.h"
#include "were_thread.h"
#include "were_debug.h"

/* ================================================================================================================== */

uint64_t were_object::next_id_ = 1;

were_object::~were_object()
{
    were_debug::instance().remove_object(this);
}

were_object::were_object() :
    reference_count_(0), collapsed_(false)
{
    thread_ = were_thread::current_thread();

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

    were_object::connect(dependency, &were_object::destroyed, this_wop, [this_wop]() mutable
    {
        this_wop.collapse();
    });
}

bool were_object::same_thread() const
{
    if (thread() && thread() != were_thread::current_thread())
        return false;
    else
        return true;
}

void were_object::post(const std::function<void ()> &call)
{
    thread()->post(call);
}
