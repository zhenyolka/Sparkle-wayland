#ifndef WERE_DEBUG_H
#define WERE_DEBUG_H

#include <thread>

#ifdef X_DEBUG
#include <set>
#include <mutex>
#endif

class were_object;

class were_debug
{
public:
    ~were_debug();
    were_debug();

    void start();
    void stop();

    static void add_object(were_object *object__);
    static void remove_object(were_object *object__);
    static void print_objects();

private:
    void loop();

private:
    std::thread thread_;
    bool stop_;
#ifdef X_DEBUG
    static std::set<were_object *> object_set_;
    static std::mutex object_set_mutex_;
    static int object_count_;
#endif
};

#endif // WERE_DEBUG_H
