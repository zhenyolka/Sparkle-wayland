#include "sparkle_android.h"
#include "sparkle.h"
#include "sparkle_settings.h"

#include "sparkle_global.h" // XXX3
#include "sparkle_output.h"
#include "sparkle_compositor.h"
#include "sparkle_seat.h"
#include "sparkle_shell.h"

#include "sparkle_android_surface.h"
#include "sparkle_service.h"


sparkle_android::~sparkle_android()
{
}

sparkle_android::sparkle_android(were_object_pointer<sparkle> sparkle, were_object_pointer<sparkle_service> service) :
    sparkle_(sparkle), service_(service)
{
    MAKE_THIS_WOP

    dpi_ = sparkle->settings()->get_int("DPI", 96);

    were::connect(sparkle->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
    {
        int width = this_wop->service_->display_width();
        int height = this_wop->service_->display_height();
        int mm_width = width * 254 / (this_wop->dpi_ * 10);
        int mm_height = height * 254 / (this_wop->dpi_ * 10);

        fprintf(stdout, "display size: %dx%d %dx%d\n", width, height, mm_width, mm_height);

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
    });

    were::connect(sparkle->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        were::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object_pointer<sparkle_android_surface> android_surface(new sparkle_android_surface(this_wop, surface));
            android_surface->add_dependency(surface); // XXX2

            were::connect(this_wop, &sparkle_android::keyboard_created, android_surface, [android_surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                android_surface->register_keyboard(keyboard);
            });

            were::connect(this_wop, &sparkle_android::pointer_created, android_surface, [android_surface](were_object_pointer<sparkle_pointer> pointer)
            {
                android_surface->register_pointer(pointer);
            });

            were::connect(this_wop, &sparkle_android::touch_created, android_surface, [android_surface](were_object_pointer<sparkle_touch> touch)
            {
                android_surface->register_touch(touch);
            });

            were::emit(this_wop, &sparkle_android::android_surface_created, android_surface);
        });
    });

    were::connect(sparkle->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        were::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were::connect(this_wop, &sparkle_android::android_surface_created, keyboard, [keyboard](were_object_pointer<sparkle_android_surface> android_surface)
            {
                android_surface->register_keyboard(keyboard);
            });

            were::emit(this_wop, &sparkle_android::keyboard_created, keyboard);
        });

        were::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were::connect(this_wop, &sparkle_android::android_surface_created, pointer, [pointer](were_object_pointer<sparkle_android_surface> android_surface)
            {
                android_surface->register_pointer(pointer);
            });

            were::emit(this_wop, &sparkle_android::pointer_created, pointer);
        });

        were::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
        {
            were::connect(this_wop, &sparkle_android::android_surface_created, touch, [touch](were_object_pointer<sparkle_android_surface> android_surface)
            {
                android_surface->register_touch(touch);
            });

            were::emit(this_wop, &sparkle_android::touch_created, touch);
        });

    });
}
