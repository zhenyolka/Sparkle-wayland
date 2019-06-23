#include "sparkle_x11.h"
#include "sparkle.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_surface.h"
#include <wayland-server.h>

sparkle_x11::~sparkle_x11()
{
}

sparkle_x11::sparkle_x11(were_object_pointer<sparkle> sparkle)
{
    were::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, sparkle->output(), [](were_object_pointer<sparkle_output> output)
    {
        fprintf(stdout, "output\n");

        int width = 800;
        int height = 600;
        int mm_width = width * 254 / 960;
        int mm_height = height * 254 / 960;

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were::connect(sparkle->compositor(), &sparkle_global<sparkle_compositor>::instance, sparkle->compositor(), [](were_object_pointer<sparkle_compositor> compositor)
    {
        fprintf(stdout, "compositor\n");

        were::connect(compositor, &sparkle_compositor::surface_created, compositor, [](were_object_pointer<sparkle_surface> surface)
        {
            fprintf(stdout, "surface\n");
        });
    });
}
