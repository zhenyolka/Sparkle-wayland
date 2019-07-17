#ifndef SPARKLE_H
#define SPARKLE_H

#include "were_object_2.h"
#include "were_thread.h" // XXX3
#include "were_object_wrapper.h" // XXX3
#include <wayland-server.h>
#include <ctime>


typedef were_object_wrapper<were_object_wrapper_primitive<struct wl_display *>> sparkle_display;

template <typename T>
class sparkle_global;

class sparkle_output;
class sparkle_compositor;
class sparkle_seat;
class sparkle_shell;


class sparkle : public were_object_2, public were_thread_fd_listener, public were_thread_idle_handler
{
public:
    ~sparkle();
    sparkle();

    were_object_pointer<sparkle_global<sparkle_output>> output() const {return output_;}
    were_object_pointer<sparkle_global<sparkle_compositor>> compositor() const {return compositor_;}
    were_object_pointer<sparkle_global<sparkle_seat>> seat() const {return seat_;}
    were_object_pointer<sparkle_global<sparkle_shell>> shell() const {return shell_;}

    static uint32_t next_serial(were_object_pointer<sparkle_display> display)
    {
        return wl_display_next_serial(display->get());
    }

    static uint32_t current_msecs()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }

private:
    void event(uint32_t events);
    void idle();

private:
    were_object_pointer<sparkle_display> display_;
    were_object_pointer<sparkle_global<sparkle_output>> output_;
    were_object_pointer<sparkle_global<sparkle_compositor>> compositor_;
    were_object_pointer<sparkle_global<sparkle_seat>> seat_;
    were_object_pointer<sparkle_global<sparkle_shell>> shell_;
};

#endif // SPARKLE_H
