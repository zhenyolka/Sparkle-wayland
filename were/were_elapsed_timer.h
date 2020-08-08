#ifndef WERE_ELAPSED_TIMER_H
#define WERE_ELAPSED_TIMER_H

#include <cstdint>
#include <ctime>

class were_elapsed_timer
{
public:
    ~were_elapsed_timer();
    explicit were_elapsed_timer(clockid_t clk_id = CLOCK_MONOTONIC);

    void start();
    uint64_t elapsed_ns(bool reset = false);
    uint64_t elapsed_ms(bool reset = false);

private:
    clockid_t clk_id_;
    struct timespec ts_;
};

#endif // WERE_ELAPSED_TIMER_H
