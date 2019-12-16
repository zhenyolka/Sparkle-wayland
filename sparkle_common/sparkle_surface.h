#ifndef SPARKLE_SURFACE_H
#define SPARKLE_SURFACE_H

#include "generated/sparkle_wl_surface.h"
#include "were_surface.h"

class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;

class sparkle_surface : public sparkle_wl_surface, public were_surface
{
public:
    sparkle_surface(struct wl_client *client, int version, uint32_t id);

    void register_keyboard(were_object_pointer<sparkle_keyboard> keyboard);
    void register_pointer(were_object_pointer<sparkle_pointer> pointer);
    void register_touch(were_object_pointer<sparkle_touch> touch);

private:
    struct wl_resource *buffer_; // XXX2 Temporary
    struct wl_resource *callback_; // XXX2 Temporary
};

#endif // SPARKLE_SURFACE_H
