#include "sparkle_platform.h"
#include "sparkle.h"
#include "sparkle_settings.h"
#include "sparkle_global.h"
#include "sparkle_output.h"
#include "sparkle_shell.h"
#include "sparkle_seat.h"
#include "sparkle_platform_surface.h"
#include "were_platform_surface_provider.h"


sparkle_platform::~sparkle_platform()
{
}

sparkle_platform::sparkle_platform(were_object_pointer<sparkle> sparkle) :
    sparkle_(sparkle)
{
    MAKE_THIS_WOP

    were_object::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
    {
        int width = were_platform_surface_provider::default_provider()->display_width();
        int height = were_platform_surface_provider::default_provider()->display_height();
        int dpi = this_wop->sparkle1()->settings()->get_int("DPI", 96);
        int mm_width = width * 254 / (dpi * 10);
        int mm_height = height * 254 / (dpi * 10);

        fprintf(stdout, "display size: %dx%d %dx%d\n", width, height, mm_width, mm_height);

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were_object::connect(sparkle->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        were_object::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_platform_surface> platform_surface(new sparkle_platform_surface(this_wop, surface));
            platform_surface->add_dependency(surface); // XXX2

            were_object::connect(this_wop, &sparkle_platform::keyboard_created, platform_surface, [platform_surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                platform_surface->register_keyboard(keyboard);
            });

            were_object::connect(this_wop, &sparkle_platform::pointer_created, platform_surface, [platform_surface](were_object_pointer<sparkle_pointer> pointer)
            {
                platform_surface->register_pointer(pointer);
            });

            were_object::connect(this_wop, &sparkle_platform::touch_created, platform_surface, [platform_surface](were_object_pointer<sparkle_touch> touch)
            {
                platform_surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_platform::platform_surface_created, platform_surface);
        });
    });

    were_object::connect(sparkle->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        were_object::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were_object::connect(this_wop, &sparkle_platform::platform_surface_created, keyboard, [keyboard](were_object_pointer<sparkle_platform_surface> platform_surface)
            {
                platform_surface->register_keyboard(keyboard);
            });

            were_object::emit(this_wop, &sparkle_platform::keyboard_created, keyboard);
        });

        were_object::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were_object::connect(this_wop, &sparkle_platform::platform_surface_created, pointer, [pointer](were_object_pointer<sparkle_platform_surface> platform_surface)
            {
                platform_surface->register_pointer(pointer);
            });

            were_object::emit(this_wop, &sparkle_platform::pointer_created, pointer);
        });

        were_object::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
        {
            were_object::connect(this_wop, &sparkle_platform::platform_surface_created, touch, [touch](were_object_pointer<sparkle_platform_surface> platform_surface)
            {
                platform_surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_platform::touch_created, touch);
        });

    });
}
