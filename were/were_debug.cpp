#include "were_debug.h"
#include "were_object.h"
#include <ctime>
#include <cstdio>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>


#if X_DEBUG
const char *state_normal = "NORMAL";
const char *state_collapsed = "COLLAPSED";
const char *state_lost = "LOST";
#endif

const char *power_source = "/sys/class/power_supply/battery/current_now";

int cpu_state_read(struct cpu_state *state)
{
    FILE *file = NULL;
    char cpu[16];
    int n;

    memset(state, 0, sizeof(struct cpu_state));

    file = fopen("/proc/stat", "r");
    if (file == NULL)
        goto error;

    n = fscanf(file, "%s %lu %lu %lu %lu ", cpu, &state->user, &state->nice, &state->system, &state->idle);
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

int get_power()
{
    int fd;
    char buffer[32];
    int n;
    int current = 0;

    fd = open(power_source, O_RDONLY);
    if (fd == -1)
        goto error;

    for (int i = 0; i < 10; ++i)
    {
        lseek(fd, 0, SEEK_SET);
        n = read(fd, buffer, 32);
        if (n < 2)
            goto error;
        current += atoi(buffer);
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
    run_(false), object_count_(0), connection_count_(0), frames_(0)
{
}

void were_debug::start()
{
    if (!run_)
    {
        clock_gettime(CLOCK_MONOTONIC, &real1_);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu1_);
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
    clock_gettime(CLOCK_MONOTONIC, &real2_);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu2_);
    cpu_state_read(&state2_);

    uint64_t elapsed_real = 0;
    elapsed_real += 1000000000LL * (real2_.tv_sec - real1_.tv_sec);
    elapsed_real += real2_.tv_nsec - real1_.tv_nsec;

    uint64_t elapsed_cpu = 0;
    elapsed_cpu += 1000000000LL * (cpu2_.tv_sec - cpu1_.tv_sec);
    elapsed_cpu += cpu2_.tv_nsec - cpu1_.tv_nsec;

    float cpu_load = 100.0 * (elapsed_cpu / 1000) / (elapsed_real / 1000);

    uint64_t elapsed_cpu1 = 0;
    elapsed_cpu1 += state2_.user - state1_.user;
    elapsed_cpu1 += state2_.nice - state1_.nice;
    elapsed_cpu1 += state2_.system - state1_.system;
    elapsed_cpu1 *= 1000000000LL / sysconf(_SC_CLK_TCK);

    float load1 = 100.0 * (elapsed_cpu1 / 1000) / (elapsed_real / 1000);

    real1_ = real2_;
    cpu1_ = cpu2_;
    state1_ = state2_;

#ifdef X_DEBUG
    printf("\033[2J"); // Clear screen
    printf("\033[0;0H"); // Move cursor
#endif

    float elapsed_real_secs = 1.0 * (elapsed_real / 1000000LL) / 1000;
    float fps = 0.0;
    if (elapsed_real_secs > 0)
        fps = 1.0 * frames_ / elapsed_real_secs;
    frames_ = 0;

#ifdef X_DEBUG
    if (true)
#else
    if (object_count_ > 0)
#endif
        fprintf(stdout, "| CPU: %5.1f%% %5.1f%% | OC: %3d | CC: %3d | PWR: %4d | FPS: %2.1f |\n", cpu_load, load1, object_count_, connection_count_, get_power(), fps);

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

        const char *state = "Unknown"; // XXX2 It is already half-deleted

#if 0
        if (object__->collapsed())
            state = state_collapsed;
        else if (object__->reference_count() == 0)
            state = state_lost;
        else
            state = state_normal;
#endif

        fprintf(stdout, "%-20p%-45.44s%-5d%-10s\n", object__, typeid(*object__).name(), 0, state);

        throw were_exception(WE_SIMPLE);
    }

    object_set_.erase(search);

    object_set_mutex_.unlock();
#endif
    object_count_ -= 1;
}

void were_debug::add_connection()
{
    connection_count_ += 1;
}

void were_debug::remove_connection()
{
    connection_count_ -= 1;
}

void were_debug::frame()
{
    frames_ += 1;
}

void were_debug::print_objects()
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    fprintf(stdout, "%-20s%-45s%-5s%-10s\n", "Pointer", "Type", "RC", "State");

    for (auto &object__ : object_set_)
    {
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
