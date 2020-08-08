#include "were_elapsed_timer.h"

were_elapsed_timer::~were_elapsed_timer()
{
}

were_elapsed_timer::were_elapsed_timer(clockid_t clk_id) :
    clk_id_(clk_id), ts_({})
{
}

void were_elapsed_timer::start()
{
    clock_gettime(clk_id_, &ts_);
}

uint64_t were_elapsed_timer::elapsed_ns(bool reset)
{
    struct timespec now = {};

    clock_gettime(clk_id_, &now);

    uint64_t elapsed = 0;
    elapsed += 1000000000ULL * (now.tv_sec - ts_.tv_sec);
    elapsed += now.tv_nsec - ts_.tv_nsec;

    if (reset)
        ts_ = now;

    return elapsed;
}

uint64_t were_elapsed_timer::elapsed_ms(bool reset)
{
    return elapsed_ns(reset) / 1000000ULL;
}
