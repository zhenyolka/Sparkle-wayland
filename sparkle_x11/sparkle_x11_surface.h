#ifndef SPARKLE_X11_SURFACE_H
#define SPARKLE_X11_SURFACE_H

#include "sparkle.h"
#include <X11/Xlib.h>

class sparkle_x11;
class sparkle_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;
typedef were_object_wrapper<were_object_wrapper_primitive<Display *>> x11_display;

class sparkle_x11_surface : public were_object
{
public:
    ~sparkle_x11_surface();
    sparkle_x11_surface(were_object_pointer<sparkle_x11> x11, were_object_pointer<sparkle_surface> surface);

    were_object_pointer<sparkle_surface> surface() const
    {
        return surface_;
    }

    void register_keyboard(were_object_pointer<sparkle_keyboard> keyboard);
    void register_pointer(were_object_pointer<sparkle_pointer> pointer);
    void register_touch(were_object_pointer<sparkle_touch> touch);

signals:

    were_signal<void (int code)> key_down;
    were_signal<void (int code)> key_up;

    were_signal<void (int id, int x, int y)> touch_down;
    were_signal<void (int id, int x, int y)> touch_up;
    were_signal<void (int id, int x, int y)> touch_motion;

    were_signal<void (int button)> pointer_button_down;
    were_signal<void (int button)> pointer_button_up;
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
    struct wl_resource *buffer_; // XXX2 Temporary
    struct wl_resource *callback_; // XXX2 Temporary
};

#endif // SPARKLE_X11_SURFACE_H
