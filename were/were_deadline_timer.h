#ifndef WERE_DEADLINE_TIMER_H
#define WERE_DEADLINE_TIMER_H

#include "were_elapsed_timer.h"
#include <cstdint>

class were_deadline_timer
{
public:
    ~were_deadline_timer();
    explicit were_deadline_timer(uint64_t ms);

    uint64_t remaining_time();
    bool has_expired();

private:
    uint64_t ms_;
    were_elapsed_timer elapsed_;
};

#endif // WERE_DEADLINE_TIMER_H
