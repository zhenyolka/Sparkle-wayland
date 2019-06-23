#ifndef SPARKLE_COMPOSITOR_H
#define SPARKLE_COMPOSITOR_H

#include "sparkle_wl_compositor.h"
#include "sparkle_wl_surface.h"
#include "sparkle_wl_region.h"
#include "were_thread.h" // XXX


class sparkle_wl_surface;

class sparkle_compositor : public sparkle_wl_compositor
{
public:
    sparkle_compositor(struct wl_client *client, int version, uint32_t id) :
        sparkle_wl_compositor(client, version, id)
    {
        MAKE_THIS_WOP

        were::connect(this_wop, &sparkle_compositor::create_surface, this_wop, [this_wop](uint32_t id)
        {
            were_object_pointer<sparkle_wl_surface> surface(new sparkle_wl_surface(this_wop->client(), this_wop->version(), id));
            were::emit(this_wop, &sparkle_compositor::surface_created, surface);
        });

        were::connect(this_wop, &sparkle_compositor::create_region, this_wop, [this_wop](uint32_t id)
        {
            were_object_pointer<sparkle_wl_region> region(new sparkle_wl_region(this_wop->client(), this_wop->version(), id));
            were::emit(this_wop, &sparkle_compositor::region_created, region);
        });
    }

signals:
    were_signal<void (were_object_pointer<sparkle_wl_surface> surface)> surface_created;
    were_signal<void (were_object_pointer<sparkle_wl_region> region)> region_created;
};

#endif // SPARKLE_COMPOSITOR_H
