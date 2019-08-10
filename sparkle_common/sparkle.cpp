#include "sparkle.h"

#include "sparkle_settings.h"
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
    //thread()->remove_idle_handler(this); // XXX1
    //struct wl_event_loop *loop = wl_display_get_event_loop(display_->get());
    //int fd = wl_event_loop_get_fd(loop);
    //thread()->remove_fd_listener(fd); // XXX1

    shell_.collapse();
    seat_.collapse();
    compositor_.collapse();
    output_.collapse();
    display_.collapse();

    settings_.collapse();
}

sparkle::sparkle(const std::string &home_dir)
{
    MAKE_THIS_WOP

    if (!home_dir.empty())
        settings_ = were_object_pointer<sparkle_settings>(new sparkle_settings(home_dir + "/settings.lua"));
    else
        settings_ = were_object_pointer<sparkle_settings>(new sparkle_settings("settings.lua"));

    display_ = were_object_pointer<sparkle_display>(new sparkle_display(wl_display_create()));
    display_->set_destructor([](struct wl_display *&display)
    {
        wl_display_destroy(display);
    });

    wl_display_init_shm(display_->get());

    if (!home_dir.empty())
        setenv("XDG_RUNTIME_DIR", home_dir.c_str(), 1);

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
