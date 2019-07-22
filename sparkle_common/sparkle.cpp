#include "sparkle.h"
#include "sparkle_global.h"

#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_seat.h"
#include "sparkle_shell.h"

#include <wayland-server.h>
#include <sys/stat.h>


#include <cstdio>


sparkle::~sparkle()
{
    thread()->remove_idle_handler(this);
    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    int fd = wl_event_loop_get_fd(loop);
    thread()->remove_fd_listener(fd);

    shell_->collapse();
    seat_->collapse();
    compositor_->collapse();
    output_->collapse();
    display_->collapse();
}

sparkle::sparkle()
{
    MAKE_THIS_WOP

    display_ = were_object_pointer<sparkle_display>(new sparkle_display(wl_display_create()));
    display_->set_destructor([](struct wl_display *&display)
    {
        wl_display_destroy(display);
    });

    wl_display_init_shm(display_->get());
#ifdef __ANDROID__
    setenv("XDG_RUNTIME_DIR", "/data/data/com.sion.sparkle", 1);
#endif
    wl_display_add_socket_auto(display_->get());
#ifdef __ANDROID__
    chmod("/data/data/com.sion.sparkle", 0755); // XXX2
    chmod("/data/data/com.sion.sparkle/wayland-0", 0666);
#endif

    output_ = were_object_pointer<sparkle_global<sparkle_output>>(new sparkle_global<sparkle_output>(display_, &wl_output_interface, 3));
    compositor_ = were_object_pointer<sparkle_global<sparkle_compositor>>(new sparkle_global<sparkle_compositor>(display_, &wl_compositor_interface, 4));
    seat_ = were_object_pointer<sparkle_global<sparkle_seat>>(new sparkle_global<sparkle_seat>(display_, &wl_seat_interface, 5));
    shell_ = were_object_pointer<sparkle_global<sparkle_shell>>(new sparkle_global<sparkle_shell>(display_, &wl_shell_interface, 1));

    struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    int fd = wl_event_loop_get_fd(loop);
    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this);
    thread()->add_idle_handler(this);
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
