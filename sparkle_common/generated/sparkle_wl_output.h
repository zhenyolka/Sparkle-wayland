#ifndef SPARKLE_WL_OUTPUT_H
#define SPARKLE_WL_OUTPUT_H

#include "sparkle_resource.h"

class sparkle_wl_output : public sparkle_resource
{
public:
    ~sparkle_wl_output();
    sparkle_wl_output(struct wl_client *client, int version, uint32_t id);

    void send_geometry(int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform);
    void send_mode(uint32_t flags, int32_t width, int32_t height, int32_t refresh);
    void send_done();
    void send_scale(int32_t factor);

signals:
    were_signal<void ()> release;

};

#endif // SPARKLE_WL_OUTPUT_H
#ifndef SPARKLE_OUTPUT_H
#define SPARKLE_OUTPUT_H

class sparkle_output
{
public:
};

#endif // SPARKLE_OUTPUT_H
