#include "sparkle_wl_callback.h"
#include "sparkle_bouncer.h"

sparkle_wl_callback::~sparkle_wl_callback()
{
}

sparkle_wl_callback::sparkle_wl_callback(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_callback_interface, version, id, nullptr)
{
}
sparkle_wl_callback::sparkle_wl_callback(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_callback::send_done(uint32_t callback_data)
{
    if (valid())
        wl_callback_send_done(resource(), callback_data);
}
