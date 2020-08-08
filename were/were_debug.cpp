#include "were_debug.h"
#include "were_exception.h"
#include "were_capability_debug.h"
#include <cstdio>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>



static const char *power_source = "/sys/class/power_supply/battery/current_now";

static int cpu_state_read(cpu_state *state)
{
    FILE *file = NULL;
    char cpu[16]; // NOLINT
    int n = 0;

    file = fopen("/proc/stat", "re");
    if (file == NULL)
        goto error;

    n = fscanf(file, "%s %lu %lu %lu %lu ", cpu, &state->user, &state->nice, &state->system, &state->idle); // NOLINT
    if (n != 5)
        goto error;

    if (strcmp(cpu, "cpu") != 0)
        goto error;

    if (file)
        fclose(file);
    return 0;

error:
    if (file)
        fclose(file);
    return -1;
}

static int get_power()
{
    int fd = 0;
    char buffer[32]; // NOLINT
    int n = 0;
    int current = 0;

    fd = open(power_source, O_RDONLY | O_CLOEXEC); // NOLINT(hicpp-signed-bitwise)
    if (fd == -1)
        goto error;

    for (int i = 0; i < 10; ++i)
    {
        lseek(fd, 0, SEEK_SET);
        n = read(fd, buffer, 32);
        if (n < 2)
            goto error;
        current += static_cast<int>(strtol(buffer, nullptr, 10));
        usleep(10000);
    }

    current /= 10;

    if (fd != -1)
        close(fd);
    return current / 1000;

error:
    if (fd != -1)
        close(fd);
    return -1;
}

were_debug::~were_debug()
{
}

were_debug::were_debug() :
    cpu_(CLOCK_PROCESS_CPUTIME_ID), run_(false), object_count_(0), frames_(0)
{
}

void were_debug::start()
{
    if (!run_)
    {
        real_.start();
        cpu_.start();
        cpu_state_read(&state1_);

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
    uint64_t elapsed_real = real_.elapsed_ns(true);
    uint64_t elapsed_cpu = cpu_.elapsed_ns(true);
    cpu_state_read(&state2_);

    float cpu_load = 100.0f * (static_cast<float>(elapsed_cpu) / 1000) / (static_cast<float>(elapsed_real) / 1000);

    uint64_t elapsed_cpu1 = 0;
    elapsed_cpu1 += state2_.user - state1_.user;
    elapsed_cpu1 += state2_.nice - state1_.nice;
    elapsed_cpu1 += state2_.system - state1_.system;
    elapsed_cpu1 *= 1000000000LL / sysconf(_SC_CLK_TCK);

    float load1 = 100.0f * (static_cast<float>(elapsed_cpu1) / 1000) / (static_cast<float>(elapsed_real) / 1000);

    state1_ = state2_;

#ifdef X_DEBUG
    printf("\033[2J"); // Clear screen
    printf("\033[0;0H"); // Move cursor
#endif

    float elapsed_real_secs = 1.0f * (static_cast<float>(elapsed_real) / 1000000LL) / 1000;
    float fps = 0.0f;
    if (elapsed_real_secs > 0)
        fps = 1.0f * static_cast<float>(frames_) / elapsed_real_secs;
    frames_ = 0;

#ifndef X_DEBUG
    if (object_count_ > 0)
#endif
    {
        fprintf(stdout, "| CPU: %5.1f%% %5.1f%% | OC: %3d | PWR: %4d | FPS: %2.1f |\n", cpu_load, load1, object_count_, get_power(), fps);
    }

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

void were_debug::add_object(were_capability_debug *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();
    object_set_.insert(object__);
    object_set_mutex_.unlock();
#endif
    object_count_ += 1;
}

void were_debug::remove_object(were_capability_debug *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    auto search = object_set_.find(object__);
    if (search == object_set_.end())
        throw were_exception(WE_SIMPLE);

    object_set_.erase(search);

    object_set_mutex_.unlock();
#endif
    object_count_ -= 1;
}

void were_debug::frame()
{
    frames_ += 1;
}

void were_debug::print_objects()
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    for (const auto &object__ : object_set_)
    {
        std::string text = object__->dump();
        fprintf(stdout, "%s\n", text.c_str());
    }

    object_set_mutex_.unlock();
#endif
}
