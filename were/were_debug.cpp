#include "were_debug.h"
#include "were_object.h"
#include <ctime>
#include <cstdio>
#include <chrono>


were_debug::~were_debug()
{
    stop();
}

were_debug::were_debug() :
    stop_(false)
{
}

void were_debug::start()
{
    stop_ = false;
    thread_ = std::thread(&were_debug::loop, this);
}

void were_debug::stop()
{
    stop_ = true;
    if (thread_.joinable())
        thread_.join();
}

void were_debug::loop()
{
    struct timespec real1, real2;
    struct timespec cpu1, cpu2;
    int i = 0;

    clock_gettime(CLOCK_MONOTONIC, &real1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu1);

    while (!stop_)
    {
        if (i % 5 == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &real2);
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu2);

            uint64_t elapsed_real = 0;
            elapsed_real += 1000000000LL * (real2.tv_sec - real1.tv_sec);
            elapsed_real += real2.tv_nsec - real1.tv_nsec;

            uint64_t elapsed_cpu = 0;
            elapsed_cpu += 1000000000LL * (cpu2.tv_sec - cpu1.tv_sec);
            elapsed_cpu += cpu2.tv_nsec - cpu1.tv_nsec;

            float cpu_load = 100.0 * (elapsed_cpu / 1000) / (elapsed_real / 1000);

            real1 = real2;
            cpu1 = cpu2;

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

        i += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

#if X_DEBUG
    std::set<were_object *> were_debug::object_set_;
    std::mutex were_debug::object_set_mutex_;
    int were_debug::object_count_ = 0;
#endif

#ifdef X_DEBUG
const char *state_normal = "NORMAL";
const char *state_collapsed = "COLLAPSED";
const char *state_lost = "LOST";
#endif

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
