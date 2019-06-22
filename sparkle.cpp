#include "sparkle.h"
#include <wayland-server.h>

#include <cstdio>

sparkle::~sparkle()
{
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
}

void sparkle::event(uint32_t events)
{
    struct wl_event_loop *loop = wl_display_get_event_loop(display_);
    wl_event_loop_dispatch(loop, 0);
    wl_display_flush_clients(display_);
}
