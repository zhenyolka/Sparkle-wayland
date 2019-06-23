#include "sparkle.h"
#include "sparkle_global.h"
#include <wayland-server.h>

#include "sparkle_wl_output.h"
#include "sparkle_compositor.h"
#include "sparkle_wl_surface.h"
#include "sparkle_wl_region.h"

#include <cstdio>


sparkle::~sparkle()
{
    compositor_.collapse(); // FIXME Sparkle users
    output_.collapse();

    wl_display_destroy(display_);
}

sparkle::sparkle()
{
    display_ = wl_display_create();

    struct wl_event_loop *loop = wl_display_get_event_loop(display_);
    int fd = wl_event_loop_get_fd(loop);
    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this);

    wl_display_init_shm(display_);
    wl_display_add_socket_auto(display_);

    output_ = were_object_pointer<sparkle_global<sparkle_wl_output>>(new sparkle_global<sparkle_wl_output>(display_, &wl_output_interface, 3));
    were::connect(output_, &sparkle_global<sparkle_wl_output>::instance, output_, [](were_object_pointer<sparkle_wl_output> output)
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

    compositor_ = were_object_pointer<sparkle_global<sparkle_compositor>>(new sparkle_global<sparkle_compositor>(display_, &wl_compositor_interface, 4));
}

void sparkle::event(uint32_t events)
{
    struct wl_event_loop *loop = wl_display_get_event_loop(display_);
    wl_event_loop_dispatch(loop, 0);
    wl_display_flush_clients(display_);
}
