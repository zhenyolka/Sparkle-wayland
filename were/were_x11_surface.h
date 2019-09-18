#ifndef WERE_X11_SURFACE_H
#define WERE_X11_SURFACE_H

#include "were_platform_surface.h"
#include "were_object_wrapper.h"
#include "were1_xcb.h"

#define TOUCH_MODE 0
#define TOUCH_ID 0

class were_x11_surface_provider;

typedef were_object_wrapper<were_object_wrapper_primitive<struct were1_xcb_display *>> x11_display;

class were_x11_surface : public were_platform_surface
{
public:
    ~were_x11_surface();
    were_x11_surface(were_object_pointer<were_x11_surface_provider> x11_surface_provider);

    bool lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride);
    bool unlock_and_post();

private:
    void process(xcb_generic_event_t *event);

private:
    were_object_pointer<x11_display> display_;
    struct were1_xcb_window *window_;
#if TOUCH_MODE
    bool touch_down_;
#endif
};

#endif // WERE_X11_SURFACE_H
