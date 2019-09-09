#ifndef SPARKLE_X11_H
#define SPARKLE_X11_H

#include "sparkle.h"
#include "were_thread.h"
#include "were1_xcb.h"


class sparkle_x11_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;

typedef were_object_wrapper<were_object_wrapper_primitive<struct were1_xcb_display *>> x11_display;

class sparkle_x11 : public were_object, public were_thread_fd_listener
{
public:
    ~sparkle_x11();
    sparkle_x11(were_object_pointer<sparkle> sparkle);

    were_object_pointer<x11_display> display() const {return display_;}

signals:
    were_signal<void (xcb_generic_event_t *event)> event1; // XXX3
    were_signal<void (were_object_pointer<sparkle_x11_surface> x11_surface)> x11_surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_object_pointer<sparkle_touch> touch)> touch_created;


private:
    static void handler(xcb_generic_event_t *event, void *user);
    void event(uint32_t events);

private:
    were_object_pointer<x11_display> display_;
    int dpi_;
};

#endif // SPARKLE_X11_H
