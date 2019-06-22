#ifndef SPARKLE_RESOURCE_H
#define SPARKLE_RESOURCE_H

#include "were_object_2.h"
#include <wayland-server.h>

class sparkle_resource : public were_object_2
{
public:
    ~sparkle_resource();
    sparkle_resource(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id, const void *implementation);

    struct wl_resource *resource() const;
    int version() const;
    struct wl_client *client() const;

private:
    static void destroy_(struct wl_listener *listener, void *data);

private:
    struct wl_resource *resource_;
    struct wl_listener listener_;
};

#endif // SPARKLE_RESOURCE_H
