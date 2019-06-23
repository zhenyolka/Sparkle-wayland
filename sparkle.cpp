#include "sparkle.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include <wayland-server.h>
#include <cstdio>


sparkle::~sparkle()
{
    compositor_->collapse();
    output_->collapse();
    display_->collapse();
}

sparkle::sparkle()
{
    display_ = were_object_pointer<sparkle_display>(new sparkle_display(wl_display_create()));
    display_->set_destructor([](struct wl_display *&display)
    {
        wl_display_destroy(display);
    });

    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    int fd = wl_event_loop_get_fd(loop);
    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this);

    wl_display_init_shm(display_->get());
    wl_display_add_socket_auto(display_->get());

    output_ = were_object_pointer<sparkle_global<sparkle_output>>(new sparkle_global<sparkle_output>(display_, &wl_output_interface, 3));
    compositor_ = were_object_pointer<sparkle_global<sparkle_compositor>>(new sparkle_global<sparkle_compositor>(display_, &wl_compositor_interface, 4));
}

void sparkle::event(uint32_t events)
{
    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    wl_event_loop_dispatch(loop, 0);
    wl_display_flush_clients(display_->get());
}
