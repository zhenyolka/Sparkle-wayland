#ifndef SPARKLE_SURFACE_H
#define SPARKLE_SURFACE_H

#include "generated/sparkle_wl_surface.h"
#include "were_surface.h"

class sparkle_surface : public sparkle_wl_surface, public were_surface
{
public:
    sparkle_surface(struct wl_client *client, int version, uint32_t id);

private:
    struct wl_resource *buffer_; // XXX2 Temporary
    struct wl_resource *callback_; // XXX2 Temporary
};

#endif // SPARKLE_SURFACE_H
