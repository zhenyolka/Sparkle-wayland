#ifndef SPARKLE_SHELL_H
#define SPARKLE_SHELL_H

#include "sparkle.h"
#include "generated/sparkle_wl_shell.h"
#include "sparkle_shell_surface.h"

class sparkle_shell : public sparkle_wl_shell
{
public:
    sparkle_shell(struct wl_client *client, int version, uint32_t id, were_object_pointer<sparkle_display> display) :
        sparkle_wl_shell(client, version, id)
    {
        MAKE_THIS_WOP

        were::connect(this_wop, &sparkle_shell::get_shell_surface, this_wop, [this_wop](uint32_t id, struct wl_resource *surface)
        {
            were_object_pointer<sparkle_shell_surface> shell_surface(new sparkle_shell_surface(this_wop->client(), this_wop->version(), id));

            were::emit(this_wop, &sparkle_shell::shell_surface_created, shell_surface);
        });
    }

signals:
    were_signal<void (were_object_pointer<sparkle_shell_surface> shell_surface)> shell_surface_created;
};

#endif // SPARKLE_SHELL_H
