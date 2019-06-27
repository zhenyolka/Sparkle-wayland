#include "sparkle_x11.h"
#include "sparkle.h"
#include "sparkle_global.h"

#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_surface.h"
#include "sparkle_seat.h"
#include "sparkle_shell.h"

#include "sparkle_x11_surface.h"
#include <wayland-server.h>

#include <cstdio>

sparkle_x11::~sparkle_x11()
{
    timer_.collapse();
    display_.collapse();
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

        int width = 1280;
        int height = 720;
        int mm_width = width * 254 / 960;
        int mm_height = height * 254 / 960;

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

#if 0
    were::connect(sparkle->compositor(), &sparkle_global<sparkle_compositor>::instance, this_wop, [this_wop](were_object_pointer<sparkle_compositor> compositor)
    {
        fprintf(stdout, "compositor\n");

        were::connect(compositor, &sparkle_compositor::surface_created, this_wop, [this_wop](were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_x11_surface> x11_surface(new sparkle_x11_surface(this_wop, surface));
            x11_surface->add_dependency(surface); // XXX

            were::connect(this_wop, &sparkle_x11::keyboard_created, x11_surface, [x11_surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                connect_keyboard(x11_surface, keyboard);
            });

            were::connect(this_wop, &sparkle_x11::pointer_created, x11_surface, [x11_surface](were_object_pointer<sparkle_pointer> pointer)
            {
                connect_pointer(x11_surface, pointer);
            });

            were::emit(this_wop, &sparkle_x11::x11_surface_created, x11_surface);
        });
    });
#else
    were::connect(sparkle->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        fprintf(stdout, "shell\n");

        were::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_x11_surface> x11_surface(new sparkle_x11_surface(this_wop, surface));
            x11_surface->add_dependency(surface); // XXX

            were::connect(this_wop, &sparkle_x11::keyboard_created, x11_surface, [x11_surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                connect_keyboard(x11_surface, keyboard);
            });

            were::connect(this_wop, &sparkle_x11::pointer_created, x11_surface, [x11_surface](were_object_pointer<sparkle_pointer> pointer)
            {
                connect_pointer(x11_surface, pointer);
            });

            were::emit(this_wop, &sparkle_x11::x11_surface_created, x11_surface);
        });
    });
#endif

    were::connect(sparkle->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        fprintf(stdout, "seat\n");

        were::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were::connect(this_wop, &sparkle_x11::x11_surface_created, keyboard, [keyboard](were_object_pointer<sparkle_x11_surface> x11_surface)
            {
                connect_keyboard(x11_surface, keyboard);
            });

            were::emit(this_wop, &sparkle_x11::keyboard_created, keyboard);
        });

        were::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were::connect(this_wop, &sparkle_x11::x11_surface_created, pointer, [pointer](were_object_pointer<sparkle_x11_surface> x11_surface)
            {
                connect_pointer(x11_surface, pointer);
            });

            were::emit(this_wop, &sparkle_x11::pointer_created, pointer);
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

void sparkle_x11::connect_keyboard(were_object_pointer<sparkle_x11_surface> x11_surface, were_object_pointer<sparkle_keyboard> keyboard)
{
    //if (wl_resource_get_client(keyboard->resource()) != wl_resource_get_client(surface_->resource()))
    //    return; // XXX

    were::connect(x11_surface, &sparkle_x11_surface::key_press, keyboard, [keyboard, x11_surface](int code)
    {
        keyboard->enter(x11_surface->surface());
        keyboard->key_press(code);
        keyboard->leave(x11_surface->surface());
    });

    were::connect(x11_surface, &sparkle_x11_surface::key_release, keyboard, [keyboard](int code)
    {
        keyboard->key_release(code);
    });

    fprintf(stdout, "keyboard connected\n");
}

void sparkle_x11::connect_pointer(were_object_pointer<sparkle_x11_surface> x11_surface, were_object_pointer<sparkle_pointer> pointer)
{
    //if (wl_resource_get_client(keyboard->resource()) != wl_resource_get_client(surface_->resource()))
    //    return; // XXX

    were::connect(x11_surface, &sparkle_x11_surface::pointer_button_press, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were::connect(x11_surface, &sparkle_x11_surface::pointer_button_release, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were::connect(x11_surface, &sparkle_x11_surface::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were::connect(x11_surface, &sparkle_x11_surface::pointer_enter, pointer, [pointer, x11_surface]()
    {
        pointer->enter(x11_surface->surface());
    });

    were::connect(x11_surface, &sparkle_x11_surface::pointer_leave, pointer, [pointer, x11_surface]()
    {
        pointer->leave(x11_surface->surface());
    });

    fprintf(stdout, "pointer connected\n");
}
