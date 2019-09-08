#ifndef WERE_DEBUG_H
#define WERE_DEBUG_H

#include <ctime>
#include <thread>

#ifdef X_DEBUG
#include <set>
#include <mutex>
#endif

struct cpu_state
{
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
};

class were_object;

class were_debug
{
public:

    static were_debug &instance()
    {
        static were_debug instance;
        return instance;
    }

private:
    ~were_debug();
    were_debug();

public:
    were_debug(const were_debug &other) = delete;
    void operator=(const were_debug &other) = delete;

public:
    void start();
    void stop();

    void add_object(were_object *object__);
    void remove_object(were_object *object__);
    void add_connection();
    void remove_connection();
    void frame();

    void print_now();

private:
    void print_objects();
    void loop();

private:
    struct timespec real1_, real2_;
    struct timespec cpu1_, cpu2_;
    struct cpu_state state1_, state2_;
    std::thread thread_;
    bool run_;
    int object_count_;
    int connection_count_;
    int frames_;
#ifdef X_DEBUG
    std::set<were_object *> object_set_;
    std::mutex object_set_mutex_;
#endif
};

#endif // WERE_DEBUG_H
