#include "were_x11_compositor.h"
#include "were_surface_producer.h"
#include "were_x11_surface.h"
#include "were_surface.h"


were_x11_compositor::~were_x11_compositor()
{
    display_.collapse();
}

were_x11_compositor::were_x11_compositor() :
    display_(new x11_display(were1_xcb_display_open()))
{
    MAKE_THIS_WOP

    display_->set_destructor([](struct were1_xcb_display *&display)
    {
        were1_xcb_display_close(display);
    });

    int fd = were1_xcb_display_fd(display_->get());

    thread()->add_fd_listener(fd, EPOLLIN | EPOLLET, this_wop);
    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop, fd]()
    {
        this_wop->thread()->remove_fd_listener(fd, this_wop);
    });
}

void were_x11_compositor::register_producer(were_object_pointer<were_surface_producer> producer)
{
    MAKE_THIS_WOP

    were_object::connect(producer, &were_surface_producer::surface_created, this_wop, [this_wop](were_object_pointer<were_surface> surface)
    {
        were_object_pointer<were_x11_surface> x11_surface(new were_x11_surface(this_wop, surface));
        x11_surface->link(surface);
    });
}

void were_x11_compositor::handler(xcb_generic_event_t *event, void *user)
{
    were_x11_compositor *instance = reinterpret_cast<were_x11_compositor *>(user);
    were_object_pointer<were_x11_compositor> instance__(instance);

    were_object::emit(instance__, &were_x11_compositor::event1, event);
}

void were_x11_compositor::event(uint32_t events)
{
    were1_xcb_display_get_events(display_->get(), &were_x11_compositor::handler, this);
}
