#include "were_capability_collapse.h"
#include "were_connect.h"

were_capability_collapse::were_capability_collapse()
{
    add_disintegrator([this]()
    {
        auto this_wop = were_pointer(this);
        were::emit(this_wop, &were_capability_collapse::destroyed);
    });
}
