#ifndef SPARKLE_WL_POINTER_H
#define SPARKLE_WL_POINTER_H

#include "sparkle_resource.h"

class sparkle_wl_pointer : public sparkle_resource
{
public:
    ~sparkle_wl_pointer();
    sparkle_wl_pointer(struct wl_client *client, int version, uint32_t id);

    void send_enter(uint32_t serial, struct wl_resource *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
    void send_leave(uint32_t serial, struct wl_resource *surface);
    void send_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
    void send_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    void send_axis(uint32_t time, uint32_t axis, wl_fixed_t value);
    void send_frame();
    void send_axis_source(uint32_t axis_source);
    void send_axis_stop(uint32_t time, uint32_t axis);
    void send_axis_discrete(uint32_t axis, int32_t discrete);

signals:
    were_signal<void (uint32_t serial, struct wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y)> set_cursor;
    were_signal<void ()> release;

};

#endif // SPARKLE_WL_POINTER_H
