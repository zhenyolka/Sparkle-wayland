#ifndef SPARKLE_H
#define SPARKLE_H

#include "were.h"
#include "were_object_wrapper.h" // XXX3
#include <wayland-server.h>
#include <ctime>
#include <string>


typedef were_object_wrapper<were_object_wrapper_primitive<struct wl_display *>> sparkle_display;

template <typename T>
class sparkle_global;

class sparkle_output;
class sparkle_compositor;
class sparkle_seat;
class sparkle_shell;
class sparkle_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;


class sparkle : virtual public were_object, public were_thread_fd_listener, public were_thread_idle_handler
{
public:
    ~sparkle();
    sparkle(const std::string &home_dir = std::string());

    were_pointer<sparkle_global<sparkle_output>> output() const;
    were_pointer<sparkle_global<sparkle_compositor>> compositor() const;
    were_pointer<sparkle_global<sparkle_seat>> seat() const;
    were_pointer<sparkle_global<sparkle_shell>> shell() const;

    static uint32_t next_serial(were_pointer<sparkle_display> display)
    {
        return wl_display_next_serial(display->get());
    }

    static uint32_t current_msecs()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }

    void set_size(int width, int height)
    {
        width_ = width;
        height_ = height;
    }

signals:
    were_signal<void (were_pointer<sparkle_surface> surface)> surface_created;
    were_signal<void (were_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_pointer<sparkle_touch> touch)> touch_created;

private:
    void event(uint32_t events);
    void idle();

private:
    were_pointer<sparkle_display> display_;
    were_pointer<sparkle_global<sparkle_output>> output_;
    were_pointer<sparkle_global<sparkle_compositor>> compositor_;
    were_pointer<sparkle_global<sparkle_seat>> seat_;
    were_pointer<sparkle_global<sparkle_shell>> shell_;
    int width_;
    int height_;
};

#endif // SPARKLE_H
