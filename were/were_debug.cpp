#include "were_debug.h"
#include "were_object.h"
#include <ctime>
#include <cstdio>
#include <chrono>


#if X_DEBUG
const char *state_normal = "NORMAL";
const char *state_collapsed = "COLLAPSED";
const char *state_lost = "LOST";
#endif


were_debug::~were_debug()
{
}

were_debug::were_debug() :
    run_(false), object_count_(0)
{
}

void were_debug::start()
{
    if (!run_)
    {
        clock_gettime(CLOCK_MONOTONIC, &real1_);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu1_);

        run_ = true;
        thread_ = std::thread(&were_debug::loop, this);
    }
}

void were_debug::stop()
{
    if (run_)
    {
        run_ = false;
        if (thread_.joinable())
            thread_.join();
    }
}

void were_debug::print_now()
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

    fprintf(stdout, "CPU: %.1f%%, Object count: %d.\n", cpu_load, object_count_);
#ifdef X_DEBUG
    print_objects();
    fprintf(stdout, "\n");
#endif
}

void were_debug::loop()
{
    int i = 0;

    while (run_)
    {
        if (i % 5 == 0)
            print_now();

        i += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    print_now();
}

void were_debug::add_object(were_object *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();
    object_set_.insert(object__);
    object_set_mutex_.unlock();
#endif
    object_count_ += 1;
}

void were_debug::remove_object(were_object *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    auto search = object_set_.find(object__);
    if (search == object_set_.end())
    {
        const char *state;

        if (object__->collapsed())
            state = state_collapsed;
        else if (object__->reference_count() == 0)
            state = state_lost;
        else
            state = state_normal;

        fprintf(stdout, "%-20p%-45.44s%-5d%-10s\n", object__, typeid(*object__).name(), object__->reference_count(), state);

        throw were_exception(WE_SIMPLE);
    }

    object_set_.erase(search);

    object_set_mutex_.unlock();
#endif
    object_count_ -= 1;
}

void were_debug::print_objects()
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    fprintf(stdout, "%-20s%-45s%-5s%-10s\n", "Pointer", "Type", "RC", "State");

    for (auto it = object_set_.begin(); it != object_set_.end(); ++it)
    {
        were_object *object__ = (*it);
        const char *state;

        if (object__->collapsed())
            state = state_collapsed;
        else if (object__->reference_count() == 0)
            state = state_lost;
        else
            state = state_normal;

        fprintf(stdout, "%-20p%-45.44s%-5d%-10s\n", object__, typeid(*object__).name(), object__->reference_count(), state);
    }

    object_set_mutex_.unlock();
#endif
}
