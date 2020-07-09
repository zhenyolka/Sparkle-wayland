#include "sparkle_wl_touch.h"
#include "sparkle_bouncer.h"

static const struct wl_touch_interface interface = {
    BOUNCER_2_D(sparkle_wl_touch, release),
};

sparkle_wl_touch::~sparkle_wl_touch()
{
}

sparkle_wl_touch::sparkle_wl_touch(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_touch_interface, version, id, &interface)
{
}

sparkle_wl_touch::sparkle_wl_touch(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_touch::send_down(uint32_t serial, uint32_t time, struct wl_resource *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    if (valid())
        wl_touch_send_down(resource(), serial, time, surface, id, x, y);
}

void sparkle_wl_touch::send_up(uint32_t serial, uint32_t time, int32_t id)
{
    if (valid())
        wl_touch_send_up(resource(), serial, time, id);
}

void sparkle_wl_touch::send_motion(uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    if (valid())
        wl_touch_send_motion(resource(), time, id, x, y);
}

void sparkle_wl_touch::send_frame()
{
    if (valid())
        wl_touch_send_frame(resource());
}

void sparkle_wl_touch::send_cancel()
{
    if (valid())
        wl_touch_send_cancel(resource());
}

void sparkle_wl_touch::send_shape(int32_t id, wl_fixed_t major, wl_fixed_t minor)
{
    if (valid())
        wl_touch_send_shape(resource(), id, major, minor);
}

void sparkle_wl_touch::send_orientation(int32_t id, wl_fixed_t orientation)
{
    if (valid())
        wl_touch_send_orientation(resource(), id, orientation);
}
