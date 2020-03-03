#include "sparkle_wl_pointer.h"
#include "sparkle_bouncer.h"

static const struct wl_pointer_interface interface = {
    BOUNCER_2(sparkle_wl_pointer, set_cursor),
    BOUNCER_2_D(sparkle_wl_pointer, release),
};

sparkle_wl_pointer::~sparkle_wl_pointer()
{
}

sparkle_wl_pointer::sparkle_wl_pointer(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_pointer_interface, version, id, &interface)
{
}
sparkle_wl_pointer::sparkle_wl_pointer(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_pointer::send_enter(uint32_t serial, struct wl_resource *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    if (valid())
        wl_pointer_send_enter(resource(), serial, surface, surface_x, surface_y);
}

void sparkle_wl_pointer::send_leave(uint32_t serial, struct wl_resource *surface)
{
    if (valid())
        wl_pointer_send_leave(resource(), serial, surface);
}

void sparkle_wl_pointer::send_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    if (valid())
        wl_pointer_send_motion(resource(), time, surface_x, surface_y);
}

void sparkle_wl_pointer::send_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    if (valid())
        wl_pointer_send_button(resource(), serial, time, button, state);
}

void sparkle_wl_pointer::send_axis(uint32_t time, uint32_t axis, wl_fixed_t value)
{
    if (valid())
        wl_pointer_send_axis(resource(), time, axis, value);
}

void sparkle_wl_pointer::send_frame()
{
    if (valid())
        wl_pointer_send_frame(resource());
}

void sparkle_wl_pointer::send_axis_source(uint32_t axis_source)
{
    if (valid())
        wl_pointer_send_axis_source(resource(), axis_source);
}

void sparkle_wl_pointer::send_axis_stop(uint32_t time, uint32_t axis)
{
    if (valid())
        wl_pointer_send_axis_stop(resource(), time, axis);
}

void sparkle_wl_pointer::send_axis_discrete(uint32_t axis, int32_t discrete)
{
    if (valid())
        wl_pointer_send_axis_discrete(resource(), axis, discrete);
}
