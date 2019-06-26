#include "sparkle_wl_keyboard.h"
#include "sparkle_bouncer.h"

static const struct wl_keyboard_interface interface = {
    BOUNCER_2_D(sparkle_wl_keyboard, release),
};

sparkle_wl_keyboard::~sparkle_wl_keyboard()
{
}

sparkle_wl_keyboard::sparkle_wl_keyboard(struct wl_client *client, int version, uint32_t id) :
    sparkle_resource(client, &wl_keyboard_interface, version, id, &interface)
{
}

void sparkle_wl_keyboard::send_keymap(uint32_t format, int32_t fd, uint32_t size)
{
    wl_keyboard_send_keymap(resource(), format, fd, size);
}

void sparkle_wl_keyboard::send_enter(uint32_t serial, struct wl_resource *surface, struct wl_array *keys)
{
    wl_keyboard_send_enter(resource(), serial, surface, keys);
}

void sparkle_wl_keyboard::send_leave(uint32_t serial, struct wl_resource *surface)
{
    wl_keyboard_send_leave(resource(), serial, surface);
}

void sparkle_wl_keyboard::send_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    wl_keyboard_send_key(resource(), serial, time, key, state);
}

void sparkle_wl_keyboard::send_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    wl_keyboard_send_modifiers(resource(), serial, mods_depressed, mods_latched, mods_locked, group);
}

void sparkle_wl_keyboard::send_repeat_info(int32_t rate, int32_t delay)
{
    wl_keyboard_send_repeat_info(resource(), rate, delay);
}
