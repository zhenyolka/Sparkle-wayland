#include "sparkle_wl_surface.h"
#include "sparkle_bouncer.h"

static const struct wl_surface_interface interface = {
    BOUNCER_2_D(sparkle_wl_surface, destroy),
    BOUNCER_2(sparkle_wl_surface, attach),
    BOUNCER_2(sparkle_wl_surface, damage),
    BOUNCER_2(sparkle_wl_surface, frame),
    BOUNCER_2(sparkle_wl_surface, set_opaque_region),
    BOUNCER_2(sparkle_wl_surface, set_input_region),
    BOUNCER_2(sparkle_wl_surface, commit),
    BOUNCER_2(sparkle_wl_surface, set_buffer_transform),
    BOUNCER_2(sparkle_wl_surface, set_buffer_scale),
    BOUNCER_2(sparkle_wl_surface, damage_buffer),
};

sparkle_wl_surface::~sparkle_wl_surface()
{
}

sparkle_wl_surface::sparkle_wl_surface(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_surface_interface, version, id, &interface)
{
}

sparkle_wl_surface::sparkle_wl_surface(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_surface::send_enter(struct wl_resource *output)
{
    if (valid())
        wl_surface_send_enter(resource(), output);
}

void sparkle_wl_surface::send_leave(struct wl_resource *output)
{
    if (valid())
        wl_surface_send_leave(resource(), output);
}
