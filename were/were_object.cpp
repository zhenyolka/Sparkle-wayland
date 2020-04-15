#include "were_object.h"
#include "were_connect.h"
#include "were_debug.h"


were_object::~were_object()
{
}

were_object::were_object() :
    reference_count_(0), collapsed_(false),
    thread_(were_t_l_registry<were_pointer<were_thread>>::get())
{
}

void were_object::collapse()
{
    auto this_wop = were_pointer(this);

    were::emit(this_wop, &were_object::destroyed);
    collapsed_ = true;
}

void were_object::link(were_pointer<were_object> other)
{
    auto this_wop = were_pointer(this);

    were::connect(other, &were_object::destroyed, this_wop, [this_wop]() mutable
    {
        this_wop->collapse();
    });
}

std::string were_object::dump() const
{
    const char *state = "NORMAL";
    if (collapsed())
        state = "COLLAPSED";
    else if (reference_count() == 0)
        state = "LOST";

    char buffer[1024];
    snprintf(buffer, 1024, "%-20p%-45.44s%-5d%-10s", this, typeid(*this).name(), reference_count(), state);

    return std::string(buffer);
}
