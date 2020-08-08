#ifndef SPARKLE_WL_REGION_H
#define SPARKLE_WL_REGION_H

#include "sparkle_resource.h"

class sparkle_wl_region : public sparkle_resource
{
public:
    ~sparkle_wl_region() override;
    sparkle_wl_region(struct wl_client *client, int version, uint32_t id);
    explicit sparkle_wl_region(struct wl_resource *resource);


signals:
    were_signal<void ()> destroy;
    were_signal<void (int32_t x, int32_t y, int32_t width, int32_t height)> add;
    were_signal<void (int32_t x, int32_t y, int32_t width, int32_t height)> subtract;

};

#endif // SPARKLE_WL_REGION_H
