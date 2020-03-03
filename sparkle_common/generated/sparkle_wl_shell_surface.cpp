#include "sparkle_wl_shell_surface.h"
#include "sparkle_bouncer.h"

static const struct wl_shell_surface_interface interface = {
    BOUNCER_2(sparkle_wl_shell_surface, pong),
    BOUNCER_2(sparkle_wl_shell_surface, move),
    BOUNCER_2(sparkle_wl_shell_surface, resize),
    BOUNCER_2(sparkle_wl_shell_surface, set_toplevel),
    BOUNCER_2(sparkle_wl_shell_surface, set_transient),
    BOUNCER_2(sparkle_wl_shell_surface, set_fullscreen),
    BOUNCER_2(sparkle_wl_shell_surface, set_popup),
    BOUNCER_2(sparkle_wl_shell_surface, set_maximized),
    BOUNCER_2(sparkle_wl_shell_surface, set_title),
    BOUNCER_2(sparkle_wl_shell_surface, set_class),
};

sparkle_wl_shell_surface::~sparkle_wl_shell_surface()
{
}

sparkle_wl_shell_surface::sparkle_wl_shell_surface(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_shell_surface_interface, version, id, &interface)
{
}
sparkle_wl_shell_surface::sparkle_wl_shell_surface(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}

void sparkle_wl_shell_surface::send_ping(uint32_t serial)
{
    if (valid())
        wl_shell_surface_send_ping(resource(), serial);
}

void sparkle_wl_shell_surface::send_configure(uint32_t edges, int32_t width, int32_t height)
{
    if (valid())
        wl_shell_surface_send_configure(resource(), edges, width, height);
}

void sparkle_wl_shell_surface::send_popup_done()
{
    if (valid())
        wl_shell_surface_send_popup_done(resource());
}
