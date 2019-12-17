#include "were_thread.h"
#include "sparkle.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_x11_compositor.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include "sparkle_output.h"
#include "sparkle_seat.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"
#include <csignal>
#include <cstdio>



class test : public were_object
{
public:

    ~test()
    {

    }

    test()
    {
        MAKE_THIS_WOP

        were_object_pointer<sparkle> sparkle__(new sparkle());
        sparkle__->link(this_wop);

        were_object_pointer<were_x11_compositor> compositor__(new were_x11_compositor());
        compositor__->link(this_wop);

        were_object::connect(sparkle__->shell(), &sparkle_global<sparkle_shell>::instance, compositor__, [compositor__, this_wop](were_object_pointer<sparkle_shell> shell)
        {
            compositor__->register_producer(shell);

            were_object::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
            {
                were_object::connect(this_wop, &test::keyboard_created, surface, [surface](were_object_pointer<sparkle_keyboard> keyboard)
                {
                    surface->register_keyboard(keyboard);
                });

                were_object::connect(this_wop, &test::pointer_created, surface, [surface](were_object_pointer<sparkle_pointer> pointer)
                {
                    surface->register_pointer(pointer);
                });

                were_object::connect(this_wop, &test::touch_created, surface, [surface](were_object_pointer<sparkle_touch> touch)
                {
                    surface->register_touch(touch);
                });

                were_object::emit(this_wop, &test::surface_created, surface);
            });
        });

        were_object::connect(sparkle__->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
        {
            were_object::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
            {
                were_object::connect(this_wop, &test::surface_created, keyboard, [keyboard](were_object_pointer<sparkle_surface> surface)
                {
                    surface->register_keyboard(keyboard);
                });

                were_object::emit(this_wop, &test::keyboard_created, keyboard);
            });

            were_object::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
            {
                were_object::connect(this_wop, &test::surface_created, pointer, [pointer](were_object_pointer<sparkle_surface> surface)
                {
                    surface->register_pointer(pointer);
                });

                were_object::emit(this_wop, &test::pointer_created, pointer);
            });

            were_object::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
            {
                were_object::connect(this_wop, &test::surface_created, touch, [touch](were_object_pointer<sparkle_surface> surface)
                {
                    surface->register_touch(touch);
                });

                were_object::emit(this_wop, &test::touch_created, touch);
            });
        });

        were_object::connect(sparkle__->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
        {
            int width = 1280;
            int height = 720;
            int dpi = 96;
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


    }

    were_signal<void (were_object_pointer<sparkle_surface> surface)> surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_object_pointer<sparkle_touch> touch)> touch_created;
};

int main(int argc, char *argv[])
{
    were_backtrace::instance().enable();

    {
        were_object_pointer<were_thread> thread(new were_thread());
    }

    were_object_pointer<test> t(new test());


    were_object_pointer<were_signal_handler> sh(new were_signal_handler());
    were_object::connect(sh, &were_signal_handler::signal, sh, [&t, &sh](uint32_t number)
    {
        if (number == SIGINT)
        {
            sh.collapse();
            t.collapse();
        }
    });

    were_debug::instance().start();

    were_thread::current_thread()->run();

    were_thread::current_thread().collapse();

    were_debug::instance().stop();

    fprintf(stdout, "Done.\n");

    return 0;
}
