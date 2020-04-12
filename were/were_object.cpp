#include "were_object.h"
#include "were_connect.h"
#include "were_debug.h"


were_object::~were_object()
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->remove_object(this);
        were_registry<were_debug *>::unlock();
    }
}

were_object::were_object() :
    reference_count_(0), collapsed_(false),
    thread_(were_t_l_registry<were_object_pointer<were_thread>>::get())
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->add_object(this);
        were_registry<were_debug *>::unlock();
    }
}

void were_object::collapse()
{
    auto this_wop = make_wop(this);

    were::emit(this_wop, &were_object::destroyed);
    collapsed_ = true;
}

void were_object::link(were_object_pointer<were_object> other)
{
    auto this_wop = make_wop(this);

    were::connect(other, &were_object::destroyed, this_wop, [this_wop]() mutable
    {
        this_wop.collapse();
    });
}
