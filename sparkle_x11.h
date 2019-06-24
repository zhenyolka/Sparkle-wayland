#ifndef SPARKLE_X11_H
#define SPARKLE_X11_H

#include "sparkle.h"
#include "were_timer.h"
#include <X11/Xlib.h>

class sparkle_x11_surface;
class sparkle_keyboard;

typedef were_object_wrapper<were_object_wrapper_primitive<Display *>> x11_display;

class sparkle_x11 : public were_object_2
{
public:
    ~sparkle_x11();
    sparkle_x11(were_object_pointer<sparkle> sparkle);

    were_object_pointer<x11_display> display() const
    {
        return display_;
    }

signals:
    were_signal<void (XEvent event)> event; // XXX
    were_signal<void (were_object_pointer<sparkle_x11_surface> x11_surface)> x11_surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;


private:
    void timeout();
    static void connect_keyboard(were_object_pointer<sparkle_x11_surface> x11_surface, were_object_pointer<sparkle_keyboard> keyboard);

private:
    were_object_pointer<x11_display> display_;
    were_object_pointer<were_timer> timer_;
};

#endif // SPARKLE_X11_H
