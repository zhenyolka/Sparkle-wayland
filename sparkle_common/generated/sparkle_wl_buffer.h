#ifndef SPARKLE_WL_BUFFER_H
#define SPARKLE_WL_BUFFER_H

#include "sparkle_resource.h"

class sparkle_wl_buffer : public sparkle_resource
{
public:
    ~sparkle_wl_buffer() override;
    sparkle_wl_buffer(struct wl_client *client, int version, uint32_t id);
    explicit sparkle_wl_buffer(struct wl_resource *resource);

    void send_release();

signals:
    were_signal<void ()> destroy;

};

#endif // SPARKLE_WL_BUFFER_H
