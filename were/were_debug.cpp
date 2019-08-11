#include "were_debug.h"
#include "were_timer.h"
#include <cstdio>

const int PERIOD = 5000;

were_debug::~were_debug()
{
    timer_.collapse();
}

were_debug::were_debug()
{
    MAKE_THIS_WOP

    timer_ = were_object_pointer<were_timer>(new were_timer(PERIOD));
    were_object::connect(timer_, &were_timer::timeout, this_wop, [this_wop](){this_wop->timeout();});
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

#ifdef X_DEBUG
    printf("\033[2J"); // Clear screen
    printf("\033[0;0H"); // Move cursor
#endif

    fprintf(stdout, "CPU: %.1f%%, Object count: %d.\n", cpu_load, were_debug_object_count());
#ifdef X_DEBUG
    were_debug_print_objects();
    fprintf(stdout, "\n");
#endif
}
