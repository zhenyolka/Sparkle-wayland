#include "were_x11_surface_provider.h"
#include "were_x11_surface.h"


were_x11_surface_provider::~were_x11_surface_provider()
{
    display_.collapse();
}

were_x11_surface_provider::were_x11_surface_provider()
{
    MAKE_THIS_WOP

    display_ = were_object_pointer<x11_display>(new x11_display(were1_xcb_display_open()));
    if (display_->get() == nullptr)
        throw were_exception(WE_SIMPLE);
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

were_object_pointer<were_platform_surface> were_x11_surface_provider::create_surface(int width, int height, int format)
{
    MAKE_THIS_WOP

    were_object_pointer<were_x11_surface> surface(new were_x11_surface(this_wop, width, height, format));

    return surface;
}

void were_x11_surface_provider::handler(xcb_generic_event_t *event, void *user)
{
    were_x11_surface_provider *instance = reinterpret_cast<were_x11_surface_provider *>(user);
    were_object_pointer<were_x11_surface_provider> instance__(instance);

    were_object::emit(instance__, &were_x11_surface_provider::event1, event);
}

void were_x11_surface_provider::event(uint32_t events)
{
    were1_xcb_display_get_events(display_->get(), &were_x11_surface_provider::handler, this);
}
