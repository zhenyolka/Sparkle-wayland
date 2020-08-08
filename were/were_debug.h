#ifndef WERE_DEBUG_H
#define WERE_DEBUG_H

#include "were_elapsed_timer.h"
#include <thread>

#ifdef X_DEBUG
#include <set>
#include <mutex>
#endif

class cpu_state
{
public:
    cpu_state() : user(0), nice(0), system(0), idle(0) {}
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
};

class were_capability_debug;

class were_debug
{
public:
    ~were_debug();
    were_debug();

public:
    void start();
    void stop();

    void add_object(were_capability_debug *object__);
    void remove_object(were_capability_debug *object__);
    void frame();

    void print_now();

private:
    void print_objects();
    void loop();

private:
    were_elapsed_timer real_;
    were_elapsed_timer cpu_;
    cpu_state state1_, state2_;
    std::thread thread_;
    bool run_;
    int object_count_;
    int frames_;
#ifdef X_DEBUG
    std::set<were_capability_debug *> object_set_;
    std::mutex object_set_mutex_;
#endif
};

#endif // WERE_DEBUG_H
