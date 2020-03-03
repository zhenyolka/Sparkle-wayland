#include "sparkle_wl_output.h"
#include "sparkle_bouncer.h"

static const struct wl_output_interface interface = {
    BOUNCER_2_D(sparkle_wl_output, release),
};

sparkle_wl_output::~sparkle_wl_output()
{
}

sparkle_wl_output::sparkle_wl_output(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_output_interface, version, id, &interface)
{
}
sparkle_wl_output::sparkle_wl_output(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_output::send_geometry(int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform)
{
    if (valid())
        wl_output_send_geometry(resource(), x, y, physical_width, physical_height, subpixel, make, model, transform);
}

void sparkle_wl_output::send_mode(uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
    if (valid())
        wl_output_send_mode(resource(), flags, width, height, refresh);
}

void sparkle_wl_output::send_done()
{
    if (valid())
        wl_output_send_done(resource());
}

void sparkle_wl_output::send_scale(int32_t factor)
{
    if (valid())
        wl_output_send_scale(resource(), factor);
}
