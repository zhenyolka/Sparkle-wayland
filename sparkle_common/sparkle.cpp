#include "sparkle.h"

#include "were_fd.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_seat.h"
#include "sparkle_shell.h"
#include "were_registry.h"
#include "sparkle_settings.h"
#include "were_timer.h"

#include <wayland-server.h>
#include <sys/stat.h>
#include <unistd.h> // unlink


#include <cstdio>


sparkle::~sparkle()
{
    shell_->collapse();
    seat_->collapse();
    compositor_->collapse();
    output_->collapse();
    display_->collapse();
}

sparkle::sparkle(const std::string &home_dir) :
    display_(were_new<sparkle_display>(wl_display_create())),
    output_(were_new<sparkle_global<sparkle_output>>(display_, &wl_output_interface, 3)),
    compositor_(were_new<sparkle_global<sparkle_compositor>>(display_, &wl_compositor_interface, 4)),
    seat_(were_new<sparkle_global<sparkle_seat>>(display_, &wl_seat_interface, 5)),
    shell_(were_new<sparkle_global<sparkle_shell>>(display_, &wl_shell_interface, 1)),
    width_(1280), height_(720)

{
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

    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
        int fd__ = wl_event_loop_get_fd(loop);

        were_pointer<were_fd> fd = were_new<were_fd>(fd__, EPOLLIN | EPOLLET);
        were::link(fd, this_wop);

        were::connect(fd, &were_fd::event, this_wop, [this_wop](uint32_t events)
        {
            struct wl_event_loop *loop = wl_display_get_event_loop(this_wop->display_->get());
            wl_display_flush_clients(this_wop->display_->get());
            wl_event_loop_dispatch(loop, 0);
        });

        were::connect(thread(), &were_thread::idle, this_wop, [this_wop]()
        {
            wl_display_flush_clients(this_wop->display_->get());
        });

        were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            wl_display_destroy_clients(this_wop->display_->get()); // XXX2
        });

        were::connect(output_, &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_pointer<sparkle_output> output)
        {
            int width = this_wop->width_;
            int height = this_wop->height_;
            int dpi = global<sparkle_settings>()->get<int>("DPI", 96);
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

        were::connect(shell_, &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_pointer<sparkle_shell> shell)
        {
            were::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_pointer<sparkle_shell_surface> shell_surface, were_pointer<sparkle_surface> surface)
            {
                were::connect(this_wop, &sparkle::keyboard_created, surface, [surface](were_pointer<sparkle_keyboard> keyboard)
                {
                    surface->register_keyboard(keyboard);
                });

                were::connect(this_wop, &sparkle::pointer_created, surface, [surface](were_pointer<sparkle_pointer> pointer)
                {
                    surface->register_pointer(pointer);
                });

                were::connect(this_wop, &sparkle::touch_created, surface, [surface](were_pointer<sparkle_touch> touch)
                {
                    surface->register_touch(touch);
                });

                were::emit(this_wop, &sparkle::surface_created, surface);
            });
        });

        were::connect(seat_, &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_pointer<sparkle_seat> seat)
        {
            were::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_pointer<sparkle_keyboard> keyboard)
            {
                were::connect(this_wop, &sparkle::surface_created, keyboard, [keyboard](were_pointer<sparkle_surface> surface)
                {
                    surface->register_keyboard(keyboard);
                });

                were::emit(this_wop, &sparkle::keyboard_created, keyboard);
            });

            were::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_pointer<sparkle_pointer> pointer)
            {
                were::connect(this_wop, &sparkle::surface_created, pointer, [pointer](were_pointer<sparkle_surface> surface)
                {
                    surface->register_pointer(pointer);
                });

                were::emit(this_wop, &sparkle::pointer_created, pointer);
            });

            were::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_pointer<sparkle_touch> touch)
            {
                were::connect(this_wop, &sparkle::surface_created, touch, [touch](were_pointer<sparkle_surface> surface)
                {
                    surface->register_touch(touch);
                });

                were::emit(this_wop, &sparkle::touch_created, touch);
            });
        });

    #if 0
        were_pointer<were_timer> timer = were_new<were_timer>(1000, false);
        timer->link(this_wop);
        timer->start();
        were::connect(timer, &were_timer::timeout, this_wop, [this_wop]()
        {
            int clients = 0;
            int resources = 0;

            struct wl_list *list = wl_display_get_client_list(this_wop->display_->get());
            struct wl_client *client = nullptr;
            wl_client_for_each(client, list)
            {
                clients += 1;

                wl_client_for_each_resource(client,
                                [](struct wl_resource *resource, void *user_data) -> wl_iterator_result
                                {
                                    int *resources = (int *)user_data;
                                    *resources += 1;
                                    return WL_ITERATOR_CONTINUE;
                                },
                                &resources);

            }

            fprintf(stdout, "clients %d, resources %d.\n", clients, resources);
        });
    #endif
    });

}

were_pointer<sparkle_global<sparkle_output>> sparkle::output() const
{
    return output_;
}

were_pointer<sparkle_global<sparkle_compositor>> sparkle::compositor() const
{
    return compositor_;
}

were_pointer<sparkle_global<sparkle_seat>> sparkle::seat() const
{
    return seat_;
}

were_pointer<sparkle_global<sparkle_shell>> sparkle::shell() const
{
    return shell_;
}
