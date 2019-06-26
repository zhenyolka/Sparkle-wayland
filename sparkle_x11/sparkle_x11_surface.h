#ifndef SPARKLE_X11_SURFACE_H
#define SPARKLE_X11_SURFACE_H

#include "sparkle.h"
#include <X11/Xlib.h>

class sparkle_x11;
class sparkle_surface;
typedef were_object_wrapper<were_object_wrapper_primitive<Display *>> x11_display;

class sparkle_x11_surface : public were_object_2
{
public:
    ~sparkle_x11_surface();
    sparkle_x11_surface(were_object_pointer<sparkle_x11> x11, were_object_pointer<sparkle_surface> surface);

    were_object_pointer<sparkle_surface> surface() const
    {
        return surface_;
    }

signals:
    were_signal<void (int code)> key_press;
    were_signal<void (int code)> key_release;

    were_signal<void (int button)> pointer_button_press;
    were_signal<void (int button)> pointer_button_release;
    were_signal<void (int x, int y)> pointer_motion;
    were_signal<void ()> pointer_enter;
    were_signal<void ()> pointer_leave;

private:
    void process(XEvent event);
    void commit();

private:
    were_object_pointer<x11_display> display_;
    were_object_pointer<sparkle_surface> surface_;
    Window window_;
    struct wl_resource *buffer_; // XXX
};

#endif // SPARKLE_X11_SURFACE_H
