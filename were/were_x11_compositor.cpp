#include "were_x11_compositor.h"
#include "were_fd.h"
#include "were_surface_producer.h"
#include "were_x11_surface.h"
#include "were_surface.h"


were_x11_compositor::~were_x11_compositor()
{
    display_.collapse();
}

were_x11_compositor::were_x11_compositor() :
    display_(were_new<x11_display>(were1_xcb_display_open()))
{
    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        display_->set_destructor([](struct were1_xcb_display *&display)
        {
            were1_xcb_display_close(display);
        });

        int fd__ = were1_xcb_display_fd(display_->get());

        were_pointer<were_fd> fd = were_new<were_fd>(fd__, EPOLLIN | EPOLLET);
        were::link(fd, this_wop);

        were::connect(fd, &were_fd::data_in, this_wop, [this_wop]()
        {
            were1_xcb_display_get_events(this_wop->display_->get(), &were_x11_compositor::handler, this_wop.access());
        });
    });
}

void were_x11_compositor::register_producer(were_pointer<were_surface_producer> producer)
{
    auto this_wop = were_pointer(this);

    were::connect(producer, &were_surface_producer::surface_created, this_wop, [this_wop](were_pointer<were_surface> surface)
    {
        were_pointer<were_x11_surface> x11_surface = were_new<were_x11_surface>(this_wop, surface);
        were::link(x11_surface, surface);
    });
}

void were_x11_compositor::handler(xcb_generic_event_t *event, void *user)
{
    were_x11_compositor *instance = reinterpret_cast<were_x11_compositor *>(user);
    were_pointer<were_x11_compositor> instance__(instance);

    were::emit(instance__, &were_x11_compositor::event1, event);
}
