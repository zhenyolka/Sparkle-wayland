#include "sparkle_wl_region.h"
#include "sparkle_bouncer.h"

static const struct wl_region_interface interface = {
    BOUNCER_2_D(sparkle_wl_region, destroy),
    BOUNCER_2(sparkle_wl_region, add),
    BOUNCER_2(sparkle_wl_region, subtract),
};

sparkle_wl_region::~sparkle_wl_region()
{
}

sparkle_wl_region::sparkle_wl_region(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_region_interface, version, id, &interface)
{
}
sparkle_wl_region::sparkle_wl_region(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}
