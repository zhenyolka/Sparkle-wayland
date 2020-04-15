#include "were_capability_debug.h"
#include "were_registry.h"
#include "were_debug.h"

were_capability_debug::~were_capability_debug()
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->remove_object(this);
        were_registry<were_debug *>::unlock();
    }
}

were_capability_debug::were_capability_debug()
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->add_object(this);
        were_registry<were_debug *>::unlock();
    }
}

