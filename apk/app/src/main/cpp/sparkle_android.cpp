#include "sparkle_android.h"
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_shell.h"
#include "sparkle_global.h" // XXX

sparkle_android::~sparkle_android()
{
}

sparkle_android::sparkle_android(were_object_pointer<sparkle> sparkle)
{
    MAKE_THIS_WOP

    were::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, sparkle->output(), [](were_object_pointer<sparkle_output> output)
    {
        fprintf(stdout, "output\n");

        int width = 640;
        int height = 480;
        int mm_width = width * 254 / 960;
        int mm_height = height * 254 / 960;

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were::connect(sparkle->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        fprintf(stdout, "shell\n");

        were::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            fprintf(stdout, "Surface req\n");

            //were_object_pointer<sparkle_x11_surface> x11_surface(new sparkle_x11_surface(this_wop, surface));
            //x11_surface->add_dependency(surface); // XXX
            //were::emit(this_wop, &sparkle_x11::x11_surface_created, x11_surface);
        });
    });
}
