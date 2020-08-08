#ifndef SPARKLE_WL_SURFACE_H
#define SPARKLE_WL_SURFACE_H

#include "sparkle_resource.h"

class sparkle_wl_surface : public sparkle_resource
{
public:
    ~sparkle_wl_surface() override;
    sparkle_wl_surface(struct wl_client *client, int version, uint32_t id);
    explicit sparkle_wl_surface(struct wl_resource *resource);

    void send_enter(struct wl_resource *output);
    void send_leave(struct wl_resource *output);

signals:
    were_signal<void ()> destroy;
    were_signal<void (struct wl_resource *buffer, int32_t x, int32_t y)> attach;
    were_signal<void (int32_t x, int32_t y, int32_t width, int32_t height)> damage;
    were_signal<void (uint32_t callback)> frame;
    were_signal<void (struct wl_resource *region)> set_opaque_region;
    were_signal<void (struct wl_resource *region)> set_input_region;
    were_signal<void ()> commit;
    were_signal<void (int32_t transform)> set_buffer_transform;
    were_signal<void (int32_t scale)> set_buffer_scale;
    were_signal<void (int32_t x, int32_t y, int32_t width, int32_t height)> damage_buffer;

};

#endif // SPARKLE_WL_SURFACE_H
