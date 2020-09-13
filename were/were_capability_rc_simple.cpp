#include "were_capability_rc_simple.h"


were_capability_rc_simple::were_capability_rc_simple() :
    reference_count_(0)
{
}

int were_capability_rc_simple::reference_count() const
{
    return reference_count_;
}

void were_capability_rc_simple::reference()
{
    reference_count_++;
}

void were_capability_rc_simple::unreference()
{
    reference_count_--;

    if (reference_count_ == 0)
        delete this;
}
