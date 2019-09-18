#ifndef WERE_X11_SURFACE_PROVIDER_H
#define WERE_X11_SURFACE_PROVIDER_H

#include "were_thread.h"
#include "were_platform_surface_provider.h"
#include "were_object_wrapper.h"
#include "were1_xcb.h"

typedef were_object_wrapper<were_object_wrapper_primitive<struct were1_xcb_display *>> x11_display;

class were_x11_surface_provider : public were_platform_surface_provider, public were_thread_fd_listener
{
public:
    ~were_x11_surface_provider();
    were_x11_surface_provider();

    were_object_pointer<were_platform_surface> create_surface();

    were_object_pointer<x11_display> display() const { return display_; }

signals:
    were_signal<void (xcb_generic_event_t *event)> event1;

private:
    static void handler(xcb_generic_event_t *event, void *user);
    void event(uint32_t events);

private:
    were_object_pointer<x11_display> display_;
};

#endif // WERE_X11_SURFACE_PROVIDER_H
