#include "were_capability_debug.h"
#include "were_slot.h"
#include "were_debug.h"

extern were_slot<were_debug *> s_debug;

were_capability_debug::~were_capability_debug()
{
    if (s_debug.lock())
    {
        s_debug.get()->remove_object(this);
        s_debug.unlock();
    }
}

were_capability_debug::were_capability_debug()
{
    if (s_debug.lock())
    {
        s_debug.get()->add_object(this);
        s_debug.unlock();
    }
}

