#ifndef SPARKLE_WL_SEAT_H
#define SPARKLE_WL_SEAT_H

#include "sparkle_resource.h"

class sparkle_wl_seat : public sparkle_resource
{
public:
    ~sparkle_wl_seat();
    sparkle_wl_seat(struct wl_client *client, int version, uint32_t id);
    sparkle_wl_seat(struct wl_resource *resource);

    void send_capabilities(uint32_t capabilities);
    void send_name(const char *name);

signals:
    were_signal<void (uint32_t id)> get_pointer;
    were_signal<void (uint32_t id)> get_keyboard;
    were_signal<void (uint32_t id)> get_touch;
    were_signal<void ()> release;

};

#endif // SPARKLE_WL_SEAT_H
