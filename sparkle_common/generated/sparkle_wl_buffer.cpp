#include "sparkle_wl_buffer.h"
#include "sparkle_bouncer.h"

static const struct wl_buffer_interface interface = {
    BOUNCER_2_D(sparkle_wl_buffer, destroy),
};

sparkle_wl_buffer::~sparkle_wl_buffer()
{
}

sparkle_wl_buffer::sparkle_wl_buffer(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_buffer_interface, version, id, &interface)
{
}

sparkle_wl_buffer::sparkle_wl_buffer(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_buffer::send_release()
{
    if (valid())
        wl_buffer_send_release(resource());
}
