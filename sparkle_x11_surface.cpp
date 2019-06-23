#include "sparkle_x11_surface.h"
#include "sparkle_x11.h"

#include <cstdio>

sparkle_x11_surface::~sparkle_x11_surface()
{
    XDestroyWindow(display_->get(), window_);
}

sparkle_x11_surface::sparkle_x11_surface(were_object_pointer<sparkle_x11> x11)
{
    MAKE_THIS_WOP

    display_ = x11->display();

    int width = 800;
    int height = 600;

    window_ = XCreateSimpleWindow(display_->get(), RootWindow(display_->get(), 0), 0, 0, width, height, 1, 0, 0);
    if (!window_)
        throw were_exception(WE_SIMPLE);

    XSelectInput(display_->get(), window_,
        PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask | KeyReleaseMask |
        EnterWindowMask | LeaveWindowMask);

    if (true)
        XMapWindow(display_->get(), window_);
    else
        XUnmapWindow(display_->get(), window_);

    //compositor_->event.connect(this, &SparkleCompositorSurfaceX11::process)->link(this, true);
    were::connect(x11, &sparkle_x11::event, this_wop, [this_wop](XEvent event)
    {
        this_wop->process(event);
    });
}

void sparkle_x11_surface::process(XEvent event)
{
    fprintf(stdout, "proc\n");
}
