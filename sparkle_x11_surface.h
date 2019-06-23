#ifndef SPARKLE_X11_SURFACE_H
#define SPARKLE_X11_SURFACE_H

#include "sparkle.h"
#include <X11/Xlib.h>

class sparkle_x11;
typedef were_object_wrapper<were_object_wrapper_primitive<Display *>> x11_display;

class sparkle_x11_surface : public were_object_2
{
public:
    ~sparkle_x11_surface();
    sparkle_x11_surface(were_object_pointer<sparkle_x11> x11);

private:
    void process(XEvent event);

private:
    were_object_pointer<x11_display> display_;
    Window window_;
};

#endif // SPARKLE_X11_SURFACE_H
