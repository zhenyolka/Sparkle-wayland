#ifndef SPARKLE_COMPOSITOR_H
#define SPARKLE_COMPOSITOR_H

#include "sparkle.h"
#include "generated/sparkle_wl_compositor.h"
#include "sparkle_surface.h"
#include "sparkle_region.h"
#include "were_thread.h" // XXX3


class sparkle_compositor : public sparkle_wl_compositor
{
public:
    sparkle_compositor(struct wl_client *client, int version, uint32_t id, were_pointer<sparkle_display> display) :
        sparkle_wl_compositor(client, version, id)
    {
        add_integrator([this]()
        {
            auto this_wop = were_pointer(this);

            were::connect(this_wop, &sparkle_compositor::create_surface, this_wop, [this_wop](uint32_t id)
            {
                were_pointer<sparkle_surface> surface = were_new<sparkle_surface>(this_wop->client(), this_wop->version(), id);
                were::emit(this_wop, &sparkle_compositor::surface_created, surface);
            });

            were::connect(this_wop, &sparkle_compositor::create_region, this_wop, [this_wop](uint32_t id)
            {
                were_pointer<sparkle_region> region = were_new<sparkle_region>(this_wop->client(), this_wop->version(), id);
                were::emit(this_wop, &sparkle_compositor::region_created, region);
            });
        });
    }

signals:
    were_signal<void (were_pointer<sparkle_surface> surface)> surface_created;
    were_signal<void (were_pointer<sparkle_region> region)> region_created;
};

#endif // SPARKLE_COMPOSITOR_H
