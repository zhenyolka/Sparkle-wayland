#ifndef SPARKLE_SURFACE_H
#define SPARKLE_SURFACE_H

#include "generated/sparkle_wl_surface.h"
#include "were_surface.h"
#include <optional>

class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;
class sparkle_wl_buffer;
class sparkle_wl_callback;

class sparkle_surface : public sparkle_wl_surface, public were_surface
{
public:
    ~sparkle_surface();
    sparkle_surface(struct wl_client *client, int version, uint32_t id);

    void *data(); // XXX2 Currently also used to check if data exists
    int width();
    int height();
    int stride();
    were_surface::buffer_format format();

    void register_keyboard(were_pointer<sparkle_keyboard> keyboard);
    void register_pointer(were_pointer<sparkle_pointer> pointer);
    void register_touch(were_pointer<sparkle_touch> touch);

private:

    std::optional<were_pointer<sparkle_wl_buffer>> buffer_;
    std::optional<were_pointer<sparkle_wl_callback>> callback_;
};

#endif // SPARKLE_SURFACE_H
