#include "were_debug.h"
#include "were_timer.h"
#include <cstdio>

const int PERIOD = 2000;

were_debug::~were_debug()
{
}

were_debug::were_debug()
{
    MAKE_THIS_WOP

    timer_ = were_object_pointer<were_timer>(new were_timer(PERIOD));
    were::connect(timer_, &were_timer::timeout, this_wop, [this_wop](){this_wop->timeout();});
    timer_->start();

    clock_gettime(CLOCK_MONOTONIC, &real1_);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu1_);
}

void were_debug::timeout()
{
    clock_gettime(CLOCK_MONOTONIC, &real2_);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu2_);

    uint64_t elapsed_real = 0;
    elapsed_real += 1000000000LL * (real2_.tv_sec - real1_.tv_sec);
    elapsed_real += real2_.tv_nsec - real1_.tv_nsec;

    uint64_t elapsed_cpu = 0;
    elapsed_cpu += 1000000000LL * (cpu2_.tv_sec - cpu1_.tv_sec);
    elapsed_cpu += cpu2_.tv_nsec - cpu1_.tv_nsec;

    float cpu_load = 100.0 * (elapsed_cpu / 1000) / (elapsed_real / 1000);

    real1_ = real2_;
    cpu1_ = cpu2_;

    printf("\033[2J"); // Clear screen
    printf("\033[0;0H"); // Move cursor

    fprintf(stdout, "CPU: %.1f%%\n", cpu_load);
    were_debug_print_objects();
    fprintf(stdout, "\n");
}
