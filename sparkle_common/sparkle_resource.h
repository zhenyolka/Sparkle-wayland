#ifndef SPARKLE_RESOURCE_H
#define SPARKLE_RESOURCE_H

#include "were.h"
#include <wayland-server.h>

class sparkle_resource : virtual public were_object
{
public:
    ~sparkle_resource() override;
    sparkle_resource(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id, const void *implementation);
    explicit sparkle_resource(struct wl_resource *resource);

    struct wl_resource *resource() const;
    int version() const;
    struct wl_client *client() const;
    bool valid() const { return resource_ != nullptr; }

private:
    static void destroy_(struct wl_listener *listener, void *data);

private:
    struct wl_resource *resource_;
    struct wl_listener listener_;
};

#endif // SPARKLE_RESOURCE_H
