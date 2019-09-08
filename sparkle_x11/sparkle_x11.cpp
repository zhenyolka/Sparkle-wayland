#include "sparkle_x11.h"
#include "sparkle.h"
#include "sparkle_settings.h"

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
    display_.collapse();
}

sparkle_x11::sparkle_x11(were_object_pointer<sparkle> sparkle)
{
    MAKE_THIS_WOP

    dpi_ = sparkle->settings()->get_int("DPI", 96);

    display_ = were_object_pointer<x11_display>(new x11_display(XOpenDisplay(nullptr)));
    if (display_->get() == nullptr)
        throw were_exception(WE_SIMPLE);
    display_->set_destructor([](Display *&display)
    {
        XCloseDisplay(display);
    });

    int fd = XConnectionNumber(display_->get());

    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this_wop);
    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop, fd]()
    {
        this_wop->thread()->remove_fd_listener(fd, this_wop);
    });

    were_object::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
    {
        int width = 1280;
        int height = 720;
        int mm_width = width * 254 / (this_wop->dpi_ * 10);
        int mm_height = height * 254 / (this_wop->dpi_ * 10);

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were_object::connect(sparkle->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        were_object::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_x11_surface> x11_surface(new sparkle_x11_surface(this_wop, surface));
            x11_surface->add_dependency(surface); // XXX2

            were_object::connect(this_wop, &sparkle_x11::keyboard_created, x11_surface, [x11_surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                x11_surface->register_keyboard(keyboard);
            });

            were_object::connect(this_wop, &sparkle_x11::pointer_created, x11_surface, [x11_surface](were_object_pointer<sparkle_pointer> pointer)
            {
                x11_surface->register_pointer(pointer);
            });

            were_object::connect(this_wop, &sparkle_x11::touch_created, x11_surface, [x11_surface](were_object_pointer<sparkle_touch> touch)
            {
                x11_surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_x11::x11_surface_created, x11_surface);
        });
    });

    were_object::connect(sparkle->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        were_object::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were_object::connect(this_wop, &sparkle_x11::x11_surface_created, keyboard, [keyboard](were_object_pointer<sparkle_x11_surface> x11_surface)
            {
                x11_surface->register_keyboard(keyboard);
            });

            were_object::emit(this_wop, &sparkle_x11::keyboard_created, keyboard);
        });

        were_object::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were_object::connect(this_wop, &sparkle_x11::x11_surface_created, pointer, [pointer](were_object_pointer<sparkle_x11_surface> x11_surface)
            {
                x11_surface->register_pointer(pointer);
            });

            were_object::emit(this_wop, &sparkle_x11::pointer_created, pointer);
        });

        were_object::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
        {
            were_object::connect(this_wop, &sparkle_x11::x11_surface_created, touch, [touch](were_object_pointer<sparkle_x11_surface> x11_surface)
            {
                x11_surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_x11::touch_created, touch);
        });

    });
}

void sparkle_x11::event(uint32_t events)
{
    MAKE_THIS_WOP

    XEvent event__;

    while (XPending(display_->get()))
    {
        XNextEvent(display_->get(), &event__);
        were_object::emit(this_wop, &sparkle_x11::event1, event__);
    }
}
