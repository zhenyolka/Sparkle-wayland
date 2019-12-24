#include "were_object_base.h"
#include "were_registry.h"
#include "were_debug.h"

were_object_base::~were_object_base()
{
    were_debug *debug = were_registry<were_debug>::get();
    if (debug)
        debug->remove_object(this);
}

were_object_base::were_object_base()
{
    were_debug *debug = were_registry<were_debug>::get();
    if (debug)
        debug->add_object(this);
}
