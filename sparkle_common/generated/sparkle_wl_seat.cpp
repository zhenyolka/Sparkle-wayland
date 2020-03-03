#include "sparkle_wl_seat.h"
#include "sparkle_bouncer.h"

static const struct wl_seat_interface interface = {
    BOUNCER_2(sparkle_wl_seat, get_pointer),
    BOUNCER_2(sparkle_wl_seat, get_keyboard),
    BOUNCER_2(sparkle_wl_seat, get_touch),
    BOUNCER_2_D(sparkle_wl_seat, release),
};

sparkle_wl_seat::~sparkle_wl_seat()
{
}

sparkle_wl_seat::sparkle_wl_seat(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_seat_interface, version, id, &interface)
{
}
sparkle_wl_seat::sparkle_wl_seat(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_seat::send_capabilities(uint32_t capabilities)
{
    if (valid())
        wl_seat_send_capabilities(resource(), capabilities);
}

void sparkle_wl_seat::send_name(const char *name)
{
    if (valid())
        wl_seat_send_name(resource(), name);
}
