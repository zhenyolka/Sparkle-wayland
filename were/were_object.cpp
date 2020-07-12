#include "were_object.h"
#include "were_connect.h"
#include "were_thread.h"
#include "were_debug.h"


were_object::~were_object()
{
}

were_object::were_object() :
    thread_(were_t_l_registry<were_pointer<were_thread>>::get())
{
}

void were_object::collapse()
{
    auto this_wop = were_pointer(this);
    were::emit(this_wop, &were_object::destroyed);
}

were_pointer<were_thread> were_object::thread() const
{
    return thread_;
}

std::string were_object::dump() const
{
    char buffer[1024];
    snprintf(buffer, 1024, "%-20p%-45.44s%-5d%-10s", this, typeid(*this).name(), reference_count(), "?");

    return std::string(buffer);
}
