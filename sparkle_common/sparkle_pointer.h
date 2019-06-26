#ifndef SPARKLE_POINTER_H
#define SPARKLE_POINTER_H

#include "sparkle.h"
#include "generated/sparkle_wl_pointer.h"
#include "sparkle_surface.h"
#include <linux/input-event-codes.h>

static const int button_map[4] = {0, BTN_LEFT, BTN_MIDDLE, BTN_RIGHT};

class sparkle_pointer : public sparkle_wl_pointer
{
public:
    sparkle_pointer(struct wl_client *client, int version, uint32_t id, were_object_pointer<sparkle_display> display) :
        sparkle_wl_pointer(client, version, id), display_(display)
    {
    }

    void button_down(int button)
    {
        send_button(sparkle::next_serial(display_), sparkle::current_msecs(), button_map[button], WL_POINTER_BUTTON_STATE_PRESSED);
    }

    void button_up(int button)
    {
        send_button(sparkle::next_serial(display_), sparkle::current_msecs(), button_map[button], WL_POINTER_BUTTON_STATE_RELEASED);
    }

    void motion(int x, int y)
    {
        send_motion(sparkle::current_msecs(), wl_fixed_from_int(x), wl_fixed_from_int(y));
        if (version() >= WL_POINTER_FRAME_SINCE_VERSION)
            send_frame();
    }

    void enter(were_object_pointer<sparkle_surface> surface)
    {
        send_enter(sparkle::next_serial(display_), surface->resource(), wl_fixed_from_int(0), wl_fixed_from_int(0));
        if (version() >= WL_POINTER_FRAME_SINCE_VERSION)
            send_frame();
    }

    void leave(were_object_pointer<sparkle_surface> surface)
    {
        send_leave(sparkle::next_serial(display_), surface->resource());
        if (version() >= WL_POINTER_FRAME_SINCE_VERSION)
            send_frame();
    }

private:
    were_object_pointer<sparkle_display> display_;
};

#endif // SPARKLE_POINTER_H
