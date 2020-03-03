#ifndef SPARKLE_WL_KEYBOARD_H
#define SPARKLE_WL_KEYBOARD_H

#include "sparkle_resource.h"

class sparkle_wl_keyboard : public sparkle_resource
{
public:
    ~sparkle_wl_keyboard();
    sparkle_wl_keyboard(struct wl_client *client, int version, uint32_t id);
    sparkle_wl_keyboard(struct wl_resource *resource);

    void send_keymap(uint32_t format, int32_t fd, uint32_t size);
    void send_enter(uint32_t serial, struct wl_resource *surface, struct wl_array *keys);
    void send_leave(uint32_t serial, struct wl_resource *surface);
    void send_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    void send_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
    void send_repeat_info(int32_t rate, int32_t delay);

signals:
    were_signal<void ()> release;

};

#endif // SPARKLE_WL_KEYBOARD_H
