#ifndef SPARKLE_WL_CALLBACK_H
#define SPARKLE_WL_CALLBACK_H

#include "sparkle_resource.h"

class sparkle_wl_callback : public sparkle_resource
{
public:
    ~sparkle_wl_callback() override;
    sparkle_wl_callback(struct wl_client *client, int version, uint32_t id);
    explicit sparkle_wl_callback(struct wl_resource *resource);

    void send_done(uint32_t callback_data);

signals:

};

#endif // SPARKLE_WL_CALLBACK_H
