#ifndef SPARKLE_WL_SHELL_SURFACE_H
#define SPARKLE_WL_SHELL_SURFACE_H

#include "sparkle_resource.h"

class sparkle_wl_shell_surface : public sparkle_resource
{
public:
    ~sparkle_wl_shell_surface();
    sparkle_wl_shell_surface(struct wl_client *client, int version, uint32_t id);
    sparkle_wl_shell_surface(struct wl_resource *resource);

    void send_ping(uint32_t serial);
    void send_configure(uint32_t edges, int32_t width, int32_t height);
    void send_popup_done();

signals:
    were_signal<void (uint32_t serial)> pong;
    were_signal<void (struct wl_resource *seat, uint32_t serial)> move;
    were_signal<void (struct wl_resource *seat, uint32_t serial, uint32_t edges)> resize;
    were_signal<void ()> set_toplevel;
    were_signal<void (struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags)> set_transient;
    were_signal<void (uint32_t method, uint32_t framerate, struct wl_resource *output)> set_fullscreen;
    were_signal<void (struct wl_resource *seat, uint32_t serial, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags)> set_popup;
    were_signal<void (struct wl_resource *output)> set_maximized;
    were_signal<void (const char *title)> set_title;
    were_signal<void (const char *class_)> set_class;

};

#endif // SPARKLE_WL_SHELL_SURFACE_H
