#ifndef SPARKLE_TOUCH_H
#define SPARKLE_TOUCH_H

#include "sparkle.h"
#include "generated/sparkle_wl_touch.h"
#include "sparkle_surface.h"
#include <linux/input-event-codes.h>

class sparkle_touch : public sparkle_wl_touch
{
public:
    sparkle_touch(struct wl_client *client, int version, uint32_t id, were_pointer<sparkle_display> display) :
        sparkle_wl_touch(client, version, id), display_(display)
    {
    }

    void down(were_pointer<sparkle_surface> surface, int id, int x, int y)
    {
        send_down(sparkle::next_serial(display_), sparkle::current_msecs(), surface->resource(), id,
            wl_fixed_from_int(x), wl_fixed_from_int(y));
        send_frame();
    }

    void up(were_pointer<sparkle_surface> surface, int id, int x, int y)
    {
        send_up(sparkle::next_serial(display_), sparkle::current_msecs(), id);
        send_frame();
    }

    void motion(were_pointer<sparkle_surface> surface, int id, int x, int y)
    {
        send_motion(sparkle::current_msecs(), id,
            wl_fixed_from_int(x), wl_fixed_from_int(y));
        send_frame();
    }

private:
    were_pointer<sparkle_display> display_;
};

#endif // SPARKLE_TOUCH_H
