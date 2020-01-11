#include "were_object_base.h"
#include "were_debug.h"
#include "were_registry.h"

were_object_base::~were_object_base()
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->remove_object(this);
        were_registry<were_debug *>::unlock();
    }
}

were_object_base::were_object_base()
{
    if (were_registry<were_debug *>::lock())
    {
        were_registry<were_debug *>::get()->add_object(this);
        were_registry<were_debug *>::unlock();
    }
}
