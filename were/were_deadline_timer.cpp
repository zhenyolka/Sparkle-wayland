#include "were_deadline_timer.h"

were_deadline_timer::~were_deadline_timer()
{
}

were_deadline_timer::were_deadline_timer(uint64_t ms) :
    ms_(ms)
{
    elapsed_.start();
}

uint64_t were_deadline_timer::remaining_time()
{
    uint64_t elapsed = elapsed_.elapsed_ms();

    if (elapsed >= ms_)
        return 0ULL;
    else
        return ms_ - elapsed;
}

bool were_deadline_timer::has_expired()
{
    return remaining_time() == 0ULL;
}
