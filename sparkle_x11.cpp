#include "sparkle_x11.h"
#include "sparkle.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_surface.h"
#include "sparkle_x11_surface.h"
#include <wayland-server.h>

#include <cstdio>

sparkle_x11::~sparkle_x11()
{
}

sparkle_x11::sparkle_x11(were_object_pointer<sparkle> sparkle)
{
    MAKE_THIS_WOP

    display_ = were_object_pointer<x11_display>(new x11_display(XOpenDisplay(nullptr)));
    if (display_->get() == nullptr)
        throw were_exception(WE_SIMPLE);
    display_->set_destructor([](Display *&display)
    {
        XCloseDisplay(display);
    });

    timer_ = were_object_pointer<were_timer>(new were_timer(1000/60));
    were::connect(timer_, &were_timer::timeout, this_wop, [this_wop](){this_wop->timeout();});
    timer_->start();

    were::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, sparkle->output(), [](were_object_pointer<sparkle_output> output)
    {
        fprintf(stdout, "output\n");

        int width = 800;
        int height = 600;
        int mm_width = width * 254 / 960;
        int mm_height = height * 254 / 960;

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were::connect(sparkle->compositor(), &sparkle_global<sparkle_compositor>::instance, this_wop, [this_wop](were_object_pointer<sparkle_compositor> compositor)
    {
        fprintf(stdout, "compositor\n");

        were::connect(compositor, &sparkle_compositor::surface_created, this_wop, [this_wop](were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_x11_surface> x11_surface(new sparkle_x11_surface(this_wop));
            x11_surface->add_dependency(surface);
        });
    });
}

void sparkle_x11::timeout()
{
    MAKE_THIS_WOP

    XEvent event__;

    while (XPending(display_->get()))
    {
        XNextEvent(display_->get(), &event__);
        were::emit(this_wop, &sparkle_x11::event, event__);
    }
}
