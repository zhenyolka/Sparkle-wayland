#include "sparkle_wl_shell.h"
#include "sparkle_bouncer.h"

static const struct wl_shell_interface interface = {
    BOUNCER_2(sparkle_wl_shell, get_shell_surface),
};

sparkle_wl_shell::~sparkle_wl_shell()
{
}

sparkle_wl_shell::sparkle_wl_shell(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_shell_interface, version, id, &interface)
{
}

sparkle_wl_shell::sparkle_wl_shell(struct wl_resource *resource) :
    sparkle_resource(resource)
{
}
