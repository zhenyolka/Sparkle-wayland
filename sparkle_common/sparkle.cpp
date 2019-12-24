#include "sparkle.h"

#include "sparkle_settings.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_seat.h"
#include "sparkle_shell.h"
#include "were_registry.h"

#include <wayland-server.h>
#include <sys/stat.h>
#include <unistd.h> // unlink


#include <cstdio>


sparkle::~sparkle()
{
    //thread()->remove_idle_handler(this);
    //struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    //int fd = wl_event_loop_get_fd(loop);
    //thread()->remove_fd_listener(fd);

    shell_.collapse();
    seat_.collapse();
    compositor_.collapse();
    output_.collapse();
    display_.collapse();

    settings_.collapse();
}

sparkle::sparkle(const std::string &home_dir) :
    width_(1280), height_(720)
{
    MAKE_THIS_WOP

    if (!home_dir.empty())
        settings_ = were_object_pointer<sparkle_settings>(new sparkle_settings(home_dir + "/settings.lua"));
    else
        settings_ = were_object_pointer<sparkle_settings>(new sparkle_settings("settings.lua"));
    were_registry<sparkle_settings>::set(settings_.access());

    display_ = were_object_pointer<sparkle_display>(new sparkle_display(wl_display_create()));
    display_->set_destructor([](struct wl_display *&display)
    {
        wl_display_destroy(display);
    });

    wl_display_init_shm(display_->get());

    if (!home_dir.empty())
    {
        std::string path = home_dir + "/wayland-0";
        unlink(path.c_str());
        setenv("XDG_RUNTIME_DIR", home_dir.c_str(), 1);
    }

    if (wl_display_add_socket(display_->get(), "wayland-0") == -1)
        throw were_exception(WE_SIMPLE);

#ifdef __ANDROID__
    if (!home_dir.empty())
    {
        std::string path = home_dir + "/wayland-0";
        chmod(path.c_str(), 0666);
    }
#endif

    output_ = were_object_pointer<sparkle_global<sparkle_output>>(new sparkle_global<sparkle_output>(display_, &wl_output_interface, 3));
    compositor_ = were_object_pointer<sparkle_global<sparkle_compositor>>(new sparkle_global<sparkle_compositor>(display_, &wl_compositor_interface, 4));
    seat_ = were_object_pointer<sparkle_global<sparkle_seat>>(new sparkle_global<sparkle_seat>(display_, &wl_seat_interface, 5));
    shell_ = were_object_pointer<sparkle_global<sparkle_shell>>(new sparkle_global<sparkle_shell>(display_, &wl_shell_interface, 1));

    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    int fd = wl_event_loop_get_fd(loop);
    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this_wop);
    thread()->add_idle_handler(this_wop);

    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        struct wl_event_loop *loop = wl_display_get_event_loop(this_wop->display_->get());
        int fd = wl_event_loop_get_fd(loop);
        this_wop->thread()->remove_fd_listener(fd, this_wop);
        this_wop->thread()->remove_idle_handler(this_wop);

        wl_display_destroy_clients(this_wop->display_->get()); // XXX2
    });

    were_object::connect(output_, &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
    {
        int width = this_wop->width_;
        int height = this_wop->height_;
        int dpi = this_wop->settings_->get_int("DPI", 96);
        int mm_width = width * 254 / (dpi * 10);
        int mm_height = height * 254 / (dpi * 10);

        fprintf(stdout, "display size: %dx%d %dx%d\n", width, height, mm_width, mm_height);

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were_object::connect(shell_, &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        were_object::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object::connect(this_wop, &sparkle::keyboard_created, surface, [surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                surface->register_keyboard(keyboard);
            });

            were_object::connect(this_wop, &sparkle::pointer_created, surface, [surface](were_object_pointer<sparkle_pointer> pointer)
            {
                surface->register_pointer(pointer);
            });

            were_object::connect(this_wop, &sparkle::touch_created, surface, [surface](were_object_pointer<sparkle_touch> touch)
            {
                surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle::surface_created, surface);
        });
    });

    were_object::connect(seat_, &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        were_object::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were_object::connect(this_wop, &sparkle::surface_created, keyboard, [keyboard](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_keyboard(keyboard);
            });

            were_object::emit(this_wop, &sparkle::keyboard_created, keyboard);
        });

        were_object::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were_object::connect(this_wop, &sparkle::surface_created, pointer, [pointer](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_pointer(pointer);
            });

            were_object::emit(this_wop, &sparkle::pointer_created, pointer);
        });

        were_object::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
        {
            were_object::connect(this_wop, &sparkle::surface_created, touch, [touch](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle::touch_created, touch);
        });
    });
}

void sparkle::event(uint32_t events)
{
    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    wl_display_flush_clients(display_->get());
    wl_event_loop_dispatch(loop, 0);
}

void sparkle::idle()
{
    wl_display_flush_clients(display_->get());
}
