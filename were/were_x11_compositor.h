#ifndef WERE_X11_COMPOSITOR_H
#define WERE_X11_COMPOSITOR_H

#include "were_object.h"
#include "were_thread.h"
#include "were_object_wrapper.h"
#include "were1_xcb.h"

class were_surface_producer;

typedef were_object_wrapper<were_object_wrapper_primitive<struct were1_xcb_display *>> x11_display;

class were_x11_compositor : virtual public were_object
{
public:
    ~were_x11_compositor();
    were_x11_compositor();

    void register_producer(were_pointer<were_surface_producer> producer);
    were_pointer<x11_display> display() const { return display_; }

signals:
    were_signal<void (xcb_generic_event_t *event)> event1;

private:
    static void handler(xcb_generic_event_t *event, void *user);
    void event(uint32_t events);

private:
    were_pointer<x11_display> display_;
};

#endif // WERE_X11_COMPOSITOR_H
