#ifndef SPARKLE_WL_COMPOSITOR_H
#define SPARKLE_WL_COMPOSITOR_H

#include "sparkle_resource.h"

class sparkle_wl_compositor : public sparkle_resource
{
public:
    ~sparkle_wl_compositor();
    sparkle_wl_compositor(struct wl_client *client, int version, uint32_t id);


signals:
    were_signal<void (uint32_t id)> create_surface;
    were_signal<void (uint32_t id)> create_region;

};

#endif // SPARKLE_WL_COMPOSITOR_H
