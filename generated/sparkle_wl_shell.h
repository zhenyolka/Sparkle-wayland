#ifndef SPARKLE_WL_SHELL_H
#define SPARKLE_WL_SHELL_H

#include "sparkle_resource.h"

class sparkle_wl_shell : public sparkle_resource
{
public:
    ~sparkle_wl_shell();
    sparkle_wl_shell(struct wl_client *client, int version, uint32_t id);


signals:
    were_signal<void (uint32_t id, struct wl_resource *surface)> get_shell_surface;

};

#endif // SPARKLE_WL_SHELL_H
