#ifndef SPARKLE_H
#define SPARKLE_H

#include "were_object_2.h"
#include "were_thread.h" // XXX
#include "were_object_wrapper.h" // XXX


typedef were_object_wrapper<were_object_wrapper_primitive<struct wl_display *>> sparkle_display;

template <typename T>
class sparkle_global;

class sparkle_output;
class sparkle_compositor;


class sparkle : public were_object_2, public were_thread_fd_listener
{
public:
    ~sparkle();
    sparkle();

    were_object_pointer<sparkle_global<sparkle_output>> output() const {return output_;}
    were_object_pointer<sparkle_global<sparkle_compositor>> compositor() const {return compositor_;}

private:
    void event(uint32_t events);

private:
    were_object_pointer<sparkle_display> display_;
    were_object_pointer<sparkle_global<sparkle_output>> output_;
    were_object_pointer<sparkle_global<sparkle_compositor>> compositor_;
};

#endif // SPARKLE_H
