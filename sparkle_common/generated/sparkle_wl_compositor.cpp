#include "sparkle_wl_compositor.h"
#include "sparkle_bouncer.h"

static const struct wl_compositor_interface interface = {
    BOUNCER_2(sparkle_wl_compositor, create_surface),
    BOUNCER_2(sparkle_wl_compositor, create_region),
};

sparkle_wl_compositor::~sparkle_wl_compositor()
{
}

sparkle_wl_compositor::sparkle_wl_compositor(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_compositor_interface, version, id, &interface)
{
}

sparkle_wl_compositor::sparkle_wl_compositor(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}
