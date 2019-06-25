#ifndef SPARKLE_KEYBOARD_H
#define SPARKLE_KEYBOARD_H

#include "sparkle.h"
#include "generated/sparkle_wl_keyboard.h"
#include "sparkle_surface.h"


class sparkle_keyboard : public sparkle_wl_keyboard
{
public:

    ~sparkle_keyboard()
    {
        wl_array_release(&keys_);
    }

    sparkle_keyboard(struct wl_client *client, int version, uint32_t id, were_object_pointer<sparkle_display> display) :
        sparkle_wl_keyboard(client, version, id), display_(display)
    {
        wl_array_init(&keys_);
    }

    void key_press(int code)
    {
        send_key(sparkle::next_serial(display_), sparkle::current_msecs(), code - 8, WL_KEYBOARD_KEY_STATE_PRESSED);
    }

    void key_release(int code)
    {
        send_key(sparkle::next_serial(display_), sparkle::current_msecs(), code - 8, WL_KEYBOARD_KEY_STATE_RELEASED);
    }

    void enter(were_object_pointer<sparkle_surface> surface)
    {
        send_enter(sparkle::next_serial(display_), surface->resource(), &keys_);
    }

    void leave(were_object_pointer<sparkle_surface> surface)
    {
        send_leave(sparkle::next_serial(display_), surface->resource());
    }

private:
    were_object_pointer<sparkle_display> display_;
    struct wl_array keys_;
};

#endif // SPARKLE_KEYBOARD_H
