#ifndef SPARKLE_WL_TOUCH_H
#define SPARKLE_WL_TOUCH_H

#include "sparkle_resource.h"

class sparkle_wl_touch : public sparkle_resource
{
public:
    ~sparkle_wl_touch();
    sparkle_wl_touch(struct wl_client *client, int version, uint32_t id);

    void send_down(uint32_t serial, uint32_t time, struct wl_resource *surface, int32_t id, wl_fixed_t x, wl_fixed_t y);
    void send_up(uint32_t serial, uint32_t time, int32_t id);
    void send_motion(uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);
    void send_frame();
    void send_cancel();
    void send_shape(int32_t id, wl_fixed_t major, wl_fixed_t minor);
    void send_orientation(int32_t id, wl_fixed_t orientation);

signals:
    were_signal<void ()> release;

};

#endif // SPARKLE_WL_TOUCH_H
